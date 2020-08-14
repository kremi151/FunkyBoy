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

#include "cartridge.h"

#include <fstream>
#include <memory>
#include <cstring>

#include <util/romsizes.h>
#include <memory/mbc_none.h>
#include <memory/mbc1.h>

using namespace FunkyBoy;

Cartridge::Cartridge(): rom(nullptr), romSize(0), status(CartridgeStatus::NoROMLoaded), mbc(new MBCNone()) {
    ram = new u8[8 * 1924];
}

Cartridge::~Cartridge() {
    delete[] rom;
    delete[] ram;
}

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

size_t getCartridgeRamSize(u8 ramSizeType) {
    auto type = static_cast<CartridgeRAMSize>(ramSizeType);
    switch (type) {
        case CartridgeRAMSize::CRAM_2KB:
            return 2048;
        case CartridgeRAMSize::CRAM_8KB:
            return 8192;
        case CartridgeRAMSize::CRAM_32KB:
            return 32768;
        case CartridgeRAMSize::CRAM_64KB:
            return 65536;
        case CartridgeRAMSize::CRAM_128KB:
            return 131072;
        default:
            return 0;
    }
}

void Cartridge::loadROM(std::ifstream &file) {
    loadROM(file, false);
}

void Cartridge::loadROM(std::ifstream &file, bool strictSizeCheck) {
    if (!file.good()) {
        status = CartridgeStatus::ROMFileNotReadable;
        return;
    }

    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "Seeked a length of " << length << std::endl;

    size_t maxRomSize = romSizeInBytes(ROMSize::ROM_SIZE_4096K);
    if (length > maxRomSize) {
        std::cerr << "ROM size mismatch, seeked " << length
            << " bytes, which is more than the maximal supported size of " << maxRomSize << " bytes" << std::endl;
        status = CartridgeStatus::ROMTooBig;
        return;
    }

    // It is important to always allocate 8MB for ROMs even if they are smaller
    // Some ROMs might (because of whatever reason) switch the ROM bank to an area outside of the ROM's size.
    std::unique_ptr<u8[]> romBytes = std::make_unique<u8[]>(maxRomSize);
    memset(romBytes.get(), 0, maxRomSize * sizeof(u8));

    // TODO: Improve this so that I don't have to do this ugly to-char conversion
    auto voidPtr = static_cast<void*>(romBytes.get());
    auto romPtr = static_cast<char*>(voidPtr);
    file.read(romPtr, length);

    auto *header = reinterpret_cast<ROMHeader*>(romBytes.get());

    std::cout << "ROM title: " << header->title << std::endl;

    auto romSizeType = static_cast<ROMSize>(header->romSize);
    std::cout << "ROM size type: " << romSizeType << std::endl;

    size_t romFlagInBytes = romSizeInBytes(romSizeType);
    if (romFlagInBytes == 0) {
        std::cerr << "Unknown rom size flag: " << (header->romSize & 0xFF) << std::endl;
        status = CartridgeStatus::ROMParseError;
        return;
    } else if (strictSizeCheck && romFlagInBytes != length) {
        std::cerr << "ROM size mismatch, loaded " << length << " bytes, but expected " << romFlagInBytes << std::endl;
        status = CartridgeStatus::ROMSizeMismatch;
        return;
    }

    CartridgeRAMSize ramSizeType;
    if (header->ramSize > 0x5) {
        status = CartridgeStatus::RAMSizeUnsupported;
        return;
    }
    u8 ramSize = header->ramSize;
    if (ramSize == 0 && hasCartridgeRam(header->cartridgeType)) {
        ramSize = 1;
    }
    ramSizeType = static_cast<CartridgeRAMSize>(ramSize);

    switch (header->cartridgeType) {
        case 0x00:
            mbc = std::make_unique<MBCNone>();
            break;
        case 0x01:
            mbc = std::make_unique<MBC1>(romSizeType, MBC1RAMSize::MBC1_NoRam);
            break;
        case 0x02:
        case 0x03: {
            // TODO: Battery
            MBC1RAMSize mbc1RamSize;
            switch (ramSizeType) {
                case CRAM_None:
                    mbc1RamSize = MBC1RAMSize::MBC1_NoRam;
                    break;
                case CRAM_2KB:
                    mbc1RamSize = MBC1RAMSize::MBC1_2KByte;
                    break;
                case CRAM_8KB:
                    mbc1RamSize = MBC1RAMSize::MBC1_8KByte;
                    break;
                case CRAM_32KB:
                    mbc1RamSize = MBC1RAMSize::MBC1_32KByte;
                    break;
                default:
                    status = CartridgeStatus::ROMUnsupportedMBC;
                    return;
            }
            mbc = std::make_unique<MBC1>(romSizeType, mbc1RamSize);
            break;
        }
        default:
            status = CartridgeStatus::ROMUnsupportedMBC;
            return;
    }

    rom = romBytes.release();
    romSize = length;

    status = CartridgeStatus::Loaded;
}

void Cartridge::loadRamFromFS(std::ifstream &file) {
    auto ramSize = getCartridgeRamSize(getHeader()->ramSize);
    if (ramSize == 0) {
        return;
    }
    file.read(static_cast<char*>(static_cast<void*>(ram)), ramSize);
}

void Cartridge::writeRamToFS(std::ofstream &file) {
    auto ramSize = getCartridgeRamSize(getHeader()->ramSize);
    if (ramSize == 0) {
        return;
    }
    file.write(static_cast<char*>(static_cast<void*>(ram)), ramSize);
}

const ROMHeader * Cartridge::getHeader() {
    return reinterpret_cast<ROMHeader*>(rom);
}

CartridgeStatus Cartridge::getStatus() {
    return status;
}
