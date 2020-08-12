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

using namespace FunkyBoy;

#define FB_INTERNAL_RAM_BANK_SIZE (4 * 1024)

Memory::Memory(std::shared_ptr<Cartridge> cartridge, Controller::ControllersPtr controllers, const io_registers& ioRegisters)
    : cartridge(std::move(cartridge))
    , controllers(std::move(controllers))
    , ioRegisters(ioRegisters)
    , interruptEnableRegister(0)
    , dmaStarted(false)
{
    vram = new u8[6144]{};
    bgMapData1 = new u8[1024]{};
    bgMapData2 = new u8[1024]{};
    internalRam = new u8[8 * FB_INTERNAL_RAM_BANK_SIZE]{};
    oam = new u8[160]{};
    hram = new u8[127]{};

    dynamicRamBank = internalRam + FB_INTERNAL_RAM_BANK_SIZE;
}

Memory::~Memory() {
    delete[] vram;
    delete[] bgMapData1;
    delete[] bgMapData2;
    delete[] internalRam;
    delete[] oam;
    delete[] hram;
}

u8* Memory::getMemoryAddress(FunkyBoy::memory_address offset) {
    if (offset == FB_REG_IE) {
        return &interruptEnableRegister;
    } else if (offset >= 0xFF80) {
        return hram + (offset - 0xFF80);
    } else if (offset >= 0xFF00) {
        // Handled by interceptWrite and interceptReadAt
        fprintf(stderr, "Attempting to get access to HWIO, this means that the developer has forked something up\n");
        return nullptr;
    } else if (offset >= 0xFEA0) {
        // Not usable
        return nullptr;
    } else if (offset >= 0xFE00) {
        return oam + (offset - 0xFE00);
    } else if (offset >= 0xE000) {
        // Echo RAM
        // TODO: Verify that this offset is correctly calculated (Echo RAM offset + boundary of RAM bank 0)
        if (offset <= 0xEFFF) {
            return internalRam + (offset - 0xE000);
        } else {
            return dynamicRamBank + (offset - 0xF000);
        }
    } else if (offset >= 0xD000) {
        // TODO: Make this switchable
        return dynamicRamBank + (offset - 0xD000);
    } else if (offset >= 0xC000) {
        return internalRam + (offset - 0xC000);
    } else if (offset >= 0xA000) {
        return cartridge->mbc->getRAMMemoryAddress(offset - 0xA000, cartridge->ram);
    } else if (offset >= 0x9C00) {
        return bgMapData2 + (offset - 0x9C00);
    } else if (offset >= 0x9800) {
        return bgMapData1 + (offset - 0x9800);
    } else if (offset >= 0x8000) {
        return vram + (offset - 0x8000);
    } else {
        return cartridge->mbc->getROMMemoryAddress(offset, cartridge->rom);
    }
}

u8 Memory::read8BitsAt(memory_address offset) {
    u8 data;
    if (!interceptReadAt(offset, &data)) {
        fprintf(stderr, "Illegal 8-bit read from 0x%04X\n", offset);
        return 0;
    }
    return data;
}

i8 Memory::readSigned8BitsAt(memory_address offset) {
    u8 data;
    if (!interceptReadAt(offset, &data)) {
        fprintf(stderr, "Illegal 8-bit read from 0x%04X\n", offset);
        return 0;
    }
    u8 *uptr = &data;
    auto vptr = static_cast<void*>(uptr);
    return *static_cast<i8*>(vptr);
}

bool Memory::interceptWrite(FunkyBoy::memory_address offset, FunkyBoy::u8 &val) {
    if (offset <= 0x7FFF && cartridge->mbc->interceptWrite(offset, val)) {
        // Writing to read-only area, so we let it intercept by the MBC
        return true;
    }
    if ((offset & 0xFF00u) == 0xFF00u) {
        switch (offset) {
            case FB_REG_SC: {
                if (val == 0x81) {
                    controllers->getSerial()->sendByte(read8BitsAt(FB_REG_SB));
                }
                break;
            }
            case FB_REG_DMA: {
                dmaStarted = true;
                dmaMsb = val & 0xF1u;
                dmaLsb = 0x00;
                break;
            }
            default:
                break;
        }
        if (offset < 0xFF80) {
            // IO registers
            ioRegisters.handleMemoryWrite(offset - 0xFF00u, val);
            return true;
        }
    }
    return false;
}

bool Memory::interceptReadAt(FunkyBoy::memory_address offset, u8 *out) {
    if (offset >= 0xFF00u && offset < 0xFF80) {
        // IO registers
        *out = ioRegisters.handleMemoryRead(offset - 0xFF00);
        return true;
    }
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return false;
    }
    *out = *ptr;
    return true;
}

void Memory::write8BitsTo(memory_address offset, u8 val) {
    if (interceptWrite(offset, val)) {
        return;
    }
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        fprintf(stderr, "Illegal 8-bit write to 0x%04X => 0x%02X\n", offset, val);
        return;
    }
    *ptr = val;
}

u16 Memory::read16BitsAt(memory_address offset) {
    u8 lsb, msb;
    if (!interceptReadAt(offset, &lsb) || !interceptReadAt(offset + 1, &msb)) {
        fprintf(stderr, "Illegal 16-bit read from 0x%04X\n", offset);
        return 0;
    }
    return (msb << 8) | lsb;
}

void Memory::write16BitsTo(memory_address offset, u16 val) {
    write16BitsTo(offset, (val >> 8) & 0xff, val & 0xff);
}

void Memory::write16BitsTo(memory_address offset, u8 msb, u8 lsb) {
    if (interceptWrite(offset, lsb)) {
        return;
    }
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        fprintf(stderr, "Illegal 16-bit write to 0x%04X\n", offset);
        return;
    }
    *ptr = lsb;
    *(ptr+1) = msb;
}

fb_inline bool Memory::isDMA() {
    return dmaStarted;
}

void Memory::doDMA() {
    if (!dmaStarted) {
        return;
    }
    *(oam + dmaLsb) = read8BitsAt(Util::compose16Bits(dmaLsb, dmaMsb));
    if (++dmaLsb > 0x9F) {
        dmaStarted = false;
    }
}

#ifdef FB_TESTING

io_registers& Memory::getIoRegisters() {
    return ioRegisters;
}

CartridgePtr& Memory::getCartridge() {
    return cartridge;
}

#endif