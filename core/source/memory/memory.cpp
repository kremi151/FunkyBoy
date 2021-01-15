/**
 * Copyright 2020 Michel Kremer (kremi151)
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "memory.h"

#include <util/endianness.h>
#include <util/typedefs.h>
#include <util/debug.h>
#include <emulator/io_registers.h>
#include <cartridge/mbc_none.h>
#include <cartridge/mbc1.h>
#include <cartridge/mbc2.h>
#include <cartridge/mbc3.h>
#include <cartridge/mbc5.h>
#include <util/romsizes.h>
#include <util/ramsizes.h>

#include <cstring>
#include <exception/read_exception.h>

using namespace FunkyBoy;

#define FB_INTERNAL_RAM_BANK_SIZE (4 * 1024)
#define FB_INTERNAL_RAM_SIZE (8 * FB_INTERNAL_RAM_BANK_SIZE)
#define FB_CARTRIDGE_HEADER_SIZE 336
#define FB_HRAM_SIZE 127

Memory::Memory(Controller::ControllersPtr controllers, const io_registers& ioRegisters, const PPUMemory &ppuMemory)
    : controllers(std::move(controllers))
    , ioRegisters(ioRegisters)
    , ppuMemory(ppuMemory)
    , interruptEnableRegister(0)
    , dmaStarted(false)
    , rom(nullptr)
    , cram(nullptr)
    , ramSizeInBytes(0)
    , status(CartridgeStatus::NoROMLoaded)
    , mbc(new MBCNone())
#ifdef FB_USE_AUTOSAVE
    , cartridgeRAMWritten(false)
#endif
{
    internalRam = new u8[FB_INTERNAL_RAM_SIZE]{};
    hram = new u8[FB_HRAM_SIZE]{};

    dynamicRamBank = internalRam + FB_INTERNAL_RAM_BANK_SIZE;
}

Memory::~Memory() {
    delete[] internalRam;
    delete[] hram;
    delete[] rom;
    delete[] cram;
}

void Memory::loadROM(std::istream &stream) {
    loadROM(stream, false);
}

namespace FunkyBoy::Util {
    bool hasCartridgeRam(u8 cartridgeType) {
        switch (cartridgeType) {
            case 0x02: case 0x03:
            case 0x08: case 0x09:
            case 0x0C: case 0x0D:
            case 0x12: case 0x13:
            case 0x1A: case 0x1B:
            case 0x1D: case 0x1E:
            case 0x22:
            case 0xFF:
                return true;
            default:
                return false;
        }
    }
}

void Memory::loadROM(std::istream &stream, bool strictSizeCheck) {
    if (!stream.good()) {
#ifdef FB_DEBUG
        fprintf(stderr, "Stream is not readable\n");
#endif
        status = CartridgeStatus::ROMFileNotReadable;
        return;
    }

    stream.seekg(0, std::ios::end);
    size_t length = stream.tellg();
    stream.seekg(0, std::ios::beg);

#ifdef FB_DEBUG
    std::cout << "Seeked a length of " << length << std::endl;
#endif

    size_t maxRomSize = romSizeInBytes(ROMSize::ROM_SIZE_8M);
    if (length > maxRomSize) {
        std::cerr << "ROM size mismatch, seeked " << length
                  << " bytes, which is more than the maximal supported size of " << maxRomSize << " bytes" << std::endl;
        status = CartridgeStatus::ROMTooBig;
        return;
    }

    if (length < FB_CARTRIDGE_HEADER_SIZE) {
#ifdef FB_DEBUG
        fprintf(stderr, "ROM is smaller than the expected cartridge header\n");
#endif
        status = CartridgeStatus::ROMFileNotReadable;
        return;
    }

    // First, let's only read the ROM header (0x0000 - 0x014F)
    std::unique_ptr<u8[]> romBytes = std::make_unique<u8[]>(FB_CARTRIDGE_HEADER_SIZE);
    stream.read(reinterpret_cast<char *>(romBytes.get()), FB_CARTRIDGE_HEADER_SIZE);
    auto *header = reinterpret_cast<ROMHeader*>(romBytes.get());

    auto romSizeType = static_cast<ROMSize>(header->romSize);
    size_t romSize = romSizeInBytes(romSizeType);

    if (romSize == 0) {
        std::cerr << "Unknown rom size flag: " << (header->romSize & 0xFF) << std::endl;
        status = CartridgeStatus::ROMParseError;
        return;
    } else if (strictSizeCheck && romSize != length) {
        std::cerr << "ROM size mismatch, loaded " << length << " bytes, but expected " << romSize << std::endl;
        status = CartridgeStatus::ROMSizeMismatch;
        return;
    }
#ifdef FB_DEBUG
    else if (romSize > length) {
        fprintf(stderr, "Warning: Loading ROM which is smaller than the size it claims to have\n");
    }
#endif

    // Next, let's load the rest of the ROM
    size_t newRomSize = std::max(romSize, length);
    u8 *newRomBytes = new u8[newRomSize]{};
    std::memcpy(newRomBytes, romBytes.get(), FB_CARTRIDGE_HEADER_SIZE);
    romBytes.reset(newRomBytes);
    header = reinterpret_cast<ROMHeader*>(newRomBytes);
    stream.read(reinterpret_cast<char*>(newRomBytes) + FB_CARTRIDGE_HEADER_SIZE, length - FB_CARTRIDGE_HEADER_SIZE);

    auto type = static_cast<RAMSize>(header->ramSize);
    switch (type) {
        case RAMSize::RAM_SIZE_2KB:
            ramSizeInBytes = 2048;
            break;
        case RAMSize::RAM_SIZE_8KB:
            ramSizeInBytes = 8192;
            break;
        case RAMSize::RAM_SIZE_32KB:
            ramSizeInBytes = 32768;
            break;
        case RAMSize::RAM_SIZE_64KB:
            ramSizeInBytes = 65536;
            break;
        case RAMSize::RAM_SIZE_128KB:
            ramSizeInBytes = 131072;
            break;
        default:
            ramSizeInBytes = 0;
            break;
    }

    RAMSize ramSizeType;
    if (header->ramSize > 0x5) {
        status = CartridgeStatus::RAMSizeUnsupported;
        return;
    }
    u8 ramSize = header->ramSize;
    if (ramSize == 0 && FunkyBoy::Util::hasCartridgeRam(header->cartridgeType)) {
        ramSize = 1;
    }
    ramSizeType = static_cast<RAMSize>(ramSize);

    switch (header->cartridgeType) {
        case 0x00:
            mbc = std::make_unique<MBCNone>();
            break;
        case 0x01:
            mbc = std::make_unique<MBC1>(romSizeType, RAMSize::RAM_SIZE_None, false);
            break;
        case 0x02:
        case 0x03: {
            if (ramSizeType != RAMSize::RAM_SIZE_None
                && ramSizeType != RAMSize::RAM_SIZE_2KB
                && ramSizeType != RAMSize::RAM_SIZE_8KB
                && ramSizeType != RAMSize::RAM_SIZE_32KB) {
                status = CartridgeStatus::ROMUnsupportedMBC;
#ifdef FB_DEBUG
                fprintf(stderr, "Invalid RAM size type for MBC1: 0x%2X\n", ramSizeType);
#endif
                return;
            }
            mbc = std::make_unique<MBC1>(romSizeType, ramSizeType, header->cartridgeType == 0x03 && ramSizeType != RAMSize::RAM_SIZE_None);
            break;
        }
        case 0x05:
        case 0x06: {
            mbc = std::make_unique<MBC2>(romSizeType, header->cartridgeType == 0x06);
            ramSizeInBytes = 0x200; // MBC2 always uses a fixed RAM size of 512 bytes (4 bits per byte usable)
            break;
        }
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13: {
            bool isMBC30 = ramSizeType == RAMSize::RAM_SIZE_64KB;
            if (ramSizeType != RAMSize::RAM_SIZE_None
                && ramSizeType != RAMSize::RAM_SIZE_2KB
                && ramSizeType != RAMSize::RAM_SIZE_8KB
                && ramSizeType != RAMSize::RAM_SIZE_32KB
                && !isMBC30
            ) {
                status = CartridgeStatus::ROMUnsupportedMBC;
#ifdef FB_DEBUG
                fprintf(stderr, "Invalid RAM size type for MBC3: 0x%2X\n", ramSizeType);
#endif
                return;
            }
            bool useBattery = ramSizeType != RAMSize::RAM_SIZE_None // TODO: This actually contradicts cartridgeType 0x0f
                              && (header->cartridgeType == 0x0f
                                  || header->cartridgeType == 0x10
                                  || header->cartridgeType == 0x13);
            bool useRtc = header->cartridgeType == 0x0f || header->cartridgeType == 0x10;
            mbc = std::make_unique<MBC3>(romSizeType, ramSizeType, useBattery, useRtc, isMBC30);
            break;
        }
        case 0x19:
        case 0x1C:
            mbc = std::make_unique<MBC5>(romSizeType, RAMSize::RAM_SIZE_None, false);
            break;
        case 0x1A:
        case 0x1B:
        case 0x1D:
        case 0x1E: {
            if (ramSizeType != RAMSize::RAM_SIZE_8KB
                && ramSizeType != RAMSize::RAM_SIZE_32KB
                && ramSizeType != RAMSize::RAM_SIZE_128KB
            ) {
                status = CartridgeStatus::ROMUnsupportedMBC;
#ifdef FB_DEBUG
                fprintf(stderr, "Invalid RAM size type for MBC5: 0x%2X\n", ramSizeType);
#endif
                return;
            }
            mbc = std::make_unique<MBC5>(
                    romSizeType,
                    ramSizeType,
                    (header->cartridgeType == 0x1B || header->cartridgeType == 0x1E) && ramSizeType != RAMSize::RAM_SIZE_None
                    );
            break;
        }
        default:
            status = CartridgeStatus::ROMUnsupportedMBC;
#ifdef FB_DEBUG
            fprintf(stderr, "Unsupported MBC type: 0x%2X\n", header->cartridgeType);
#endif
            return;
    }

#ifdef FB_DEBUG
    std::cout << "ROM title: " << header->title << std::endl;
    std::cout << "ROM size type: " << romSizeType << std::endl;
    std::cout << "RAM size: " << ramSizeInBytes << " bytes (headerValue=" << (header->ramSize % 0xff) << ")" << std::endl;
#endif

    rom = romBytes.release();

    cram = new u8[ramSizeInBytes];

    status = CartridgeStatus::Loaded;
}

void Memory::loadRam(std::istream &stream) {
    if (ramSizeInBytes == 0 || !mbc->hasBattery()) {
        return;
    }
    mbc->loadBattery(stream, cram, ramSizeInBytes);
}

void Memory::writeRam(std::ostream &stream) {
    if (ramSizeInBytes == 0 || !mbc->hasBattery()) {
        return;
    }
    mbc->saveBattery(stream, cram, ramSizeInBytes);
}

const ROMHeader * Memory::getROMHeader() {
    return reinterpret_cast<ROMHeader*>(rom);
}

CartridgeStatus Memory::getCartridgeStatus() {
    return status;
}

#define FB_MEMORY_NIBBLE_RANGE(x) \
case 0x ## x ## 0: case 0x ## x ## 1: case 0x ## x ## 2: case 0x ## x ## 3: case 0x ## x ## 4: case 0x ## x ## 5: \
case 0x ## x ## 6: case 0x ## x ## 7: case 0x ## x ## 8: case 0x ## x ## 9: case 0x ## x ## A: case 0x ## x ## B: \
case 0x ## x ## C: case 0x ## x ## D: case 0x ## x ## E: case 0x ## x ## F

#define FB_MEMORY_CARTRIDGE \
FB_MEMORY_NIBBLE_RANGE(0): \
FB_MEMORY_NIBBLE_RANGE(1): \
FB_MEMORY_NIBBLE_RANGE(2): \
FB_MEMORY_NIBBLE_RANGE(3): \
FB_MEMORY_NIBBLE_RANGE(4): \
FB_MEMORY_NIBBLE_RANGE(5): \
FB_MEMORY_NIBBLE_RANGE(6): \
FB_MEMORY_NIBBLE_RANGE(7)

#define FB_MEMORY_VRAM \
FB_MEMORY_NIBBLE_RANGE(8): \
FB_MEMORY_NIBBLE_RANGE(9)

#define FB_MEMORY_CARTRIDGE_RAM \
FB_MEMORY_NIBBLE_RANGE(A): \
FB_MEMORY_NIBBLE_RANGE(B)

#define FB_MEMORY_INTERNAL_RAM \
FB_MEMORY_NIBBLE_RANGE(C)

#define FB_MEMORY_INTERNAL_RAM_DYNAMIC \
FB_MEMORY_NIBBLE_RANGE(D)

#define FB_MEMORY_ECHO_RAM \
FB_MEMORY_NIBBLE_RANGE(E)

#define FB_MEMORY_OAM \
case 0xFE

#define FB_MEMORY_ECHO_RAM_DYNAMIC \
case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: \
case 0xF7: case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD

#define FB_MEMORY_HRAM_AND_IE \
case 0xFF

u8 Memory::read8BitsAt(memory_address offset) {
    switch ((offset >> 8) & 0xff) {
        FB_MEMORY_CARTRIDGE:
            return mbc->readFromROMAt(offset, rom);
        FB_MEMORY_VRAM:
            return ppuMemory.isVRAMAccessibleFromMMU()
                   ? ppuMemory.getVRAMByte(offset - 0x8000)
                   : 0xFF;
        FB_MEMORY_CARTRIDGE_RAM:
            return mbc->readFromRAMAt(offset - 0xA000, cram);
        FB_MEMORY_INTERNAL_RAM:
            return *(internalRam + (offset - 0xC000));
        FB_MEMORY_INTERNAL_RAM_DYNAMIC:
            // TODO: Make this switchable
            return *(dynamicRamBank + (offset - 0xD000));
        FB_MEMORY_ECHO_RAM:
            return *(internalRam + (offset - 0xE000));
        FB_MEMORY_ECHO_RAM_DYNAMIC:
            return *(dynamicRamBank + (offset - 0xF000));
        FB_MEMORY_OAM: {
            if (offset < 0xFEA0) {
                return ppuMemory.isOAMAccessibleFromMMU()
                    ? ppuMemory.getOAMByte(offset - 0xFE00)
                    : 0xFF;
            } else {
                // Not usable
#if defined(FB_DEBUG)
                fprintf(stderr, "Illegal 8-bit read from 0x%04X\n", offset);
#endif
                return 0x00;
            }
        }
        FB_MEMORY_HRAM_AND_IE: {
            if (offset == FB_REG_IE) {
                return interruptEnableRegister;
            } else if (offset >= 0xFF80) {
                return *(hram + (offset - 0xFF80));
            } else {
                return ioRegisters.handleMemoryRead(offset - 0xFF00);
            }
        }
        default:
            fprintf(stderr, "Attempt to read from unmapped memory at 0x%04X\n", offset);
            return 0x00;
    }
}

void Memory::write8BitsTo(memory_address offset, u8 val) {
    switch ((offset >> 8) & 0xff) {
        FB_MEMORY_CARTRIDGE:
            // Writing to read-only area, so we let it intercept by the MBC
            mbc->interceptROMWrite(offset, val);
            break;
        FB_MEMORY_VRAM: {
            if (ppuMemory.isVRAMAccessibleFromMMU()) {
                ppuMemory.getVRAMByte(offset - 0x8000) = val;
            }
            break;
        }
        FB_MEMORY_CARTRIDGE_RAM:
#ifdef FB_USE_AUTOSAVE
            if (
#endif
                mbc->writeToRAMAt(offset - 0xA000, val, cram)
#ifdef FB_USE_AUTOSAVE
            ) {
                cartridgeRAMWritten = true;
            }
#else
                ;
#endif
            break;
        FB_MEMORY_INTERNAL_RAM:
            *(internalRam + (offset - 0xC000)) = val;
            break;
        FB_MEMORY_INTERNAL_RAM_DYNAMIC:
            // TODO: Make this switchable
            *(dynamicRamBank + (offset - 0xD000)) = val;
            break;
        FB_MEMORY_ECHO_RAM:
            *(internalRam + (offset - 0xE000)) = val;
            break;
        FB_MEMORY_ECHO_RAM_DYNAMIC:
            *(dynamicRamBank + (offset - 0xF000)) = val;
            break;
        FB_MEMORY_OAM: {
            if (offset < 0xFEA0) {
                if (ppuMemory.isOAMAccessibleFromMMU()) {
                    ppuMemory.getOAMByte(offset - 0xFE00) = val;
                }
            } else {
                // Not usable
#if defined(FB_DEBUG)
                fprintf(stderr, "Illegal 8-bit write to 0x%04X\n", offset);
#endif
            }
            break;
        }
        FB_MEMORY_HRAM_AND_IE: {
            if (offset < 0xFF80) {
                // IO registers

                if (offset == FB_REG_SC) {
                    if (val == 0x81) {
                        controllers->getSerial()->sendByte(read8BitsAt(FB_REG_SB));
                    }
                } else if (offset == FB_REG_DMA) {
                    dmaStarted = true;
                    dmaMsb = val % 0xF1u;
                    dmaLsb = 0x00;
                }

                ioRegisters.handleMemoryWrite(offset - 0xFF00u, val);
            } else {
                if (offset == FB_REG_IE) {
                    interruptEnableRegister = val;
                } else {
                    *(hram + (offset - 0xFF80)) = val;
                }
            }
            break;
        }
        default:
            fprintf(stderr, "Attempt to write to unmapped memory at 0x%04X\n", offset);
            break;
    }
}

void Memory::doDMA() {
    if (!dmaStarted) {
        return;
    }
    ppuMemory.getOAMByte(dmaLsb) = read8BitsAt(Util::compose16Bits(dmaLsb, dmaMsb));
    if (++dmaLsb > 0x9F) {
        dmaStarted = false;
    }
}

void Memory::serialize(std::ostream &ostream) const {
    ostream.write(reinterpret_cast<char*>(internalRam), FB_INTERNAL_RAM_SIZE);
    ostream.write(reinterpret_cast<char*>(hram), FB_HRAM_SIZE);
    ostream.put(interruptEnableRegister);
    ostream.put(dmaLsb);
    ostream.put(dmaMsb);
    ostream.put(dmaStarted);
    ostream.put(status);
    ostream << uint64_t(ramSizeInBytes);
    if (ramSizeInBytes > 0) {
        ostream.write(reinterpret_cast<char*>(cram), ramSizeInBytes);
    }
}

void Memory::deserialize(std::istream &istream) {
    istream.read(reinterpret_cast<char*>(internalRam), FB_INTERNAL_RAM_SIZE);
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }
    istream.read(reinterpret_cast<char*>(hram), FB_HRAM_SIZE);
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }
    char buffer[5];
    istream.read(buffer, sizeof(buffer));
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }
    interruptEnableRegister = buffer[0];
    dmaLsb = buffer[1];
    dmaMsb = buffer[2];
    dmaStarted = buffer[3] != 0;
    status = static_cast<CartridgeStatus>(buffer[4]);

    uint64_t ramSizeInBytes;
    istream >> ramSizeInBytes;

    if (ramSizeInBytes > 0) {
        if (ramSizeInBytes != this->ramSizeInBytes) {
            delete[] cram;
            cram = new u8[ramSizeInBytes];
        }
        istream.read(reinterpret_cast<char*>(cram), ramSizeInBytes);
        if (!istream) {
            throw Exception::ReadException("Stream is too short");
        }
    } else {
        delete[] cram;
    }
    this->ramSizeInBytes = ramSizeInBytes;
}

#ifdef FB_TESTING

io_registers& Memory::getIoRegisters() {
    return ioRegisters;
}

#endif