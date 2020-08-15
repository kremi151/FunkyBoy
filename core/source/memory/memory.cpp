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

Memory::Memory(std::shared_ptr<Cartridge> cartridge, Controller::ControllersPtr controllers, const io_registers& ioRegisters, const PPUMemory &ppuMemory)
    : cartridge(std::move(cartridge))
    , controllers(std::move(controllers))
    , ioRegisters(ioRegisters)
    , ppuMemory(ppuMemory)
    , interruptEnableRegister(0)
    , dmaStarted(false)
{
    internalRam = new u8[8 * FB_INTERNAL_RAM_BANK_SIZE]{};
    hram = new u8[127]{};

    dynamicRamBank = internalRam + FB_INTERNAL_RAM_BANK_SIZE;
}

Memory::~Memory() {
    delete[] internalRam;
    delete[] hram;
}

#define FB_MEMORY_NIBBLE_RANGE(x) \
case 0x ## x ## 0: case 0x ## x ## 1: case 0x ## x ## 2: case 0x ## x ## 3: case 0x ## x ## 4: case 0x ## x ## 5: \
case 0x ## x ## 6: case 0x ## x ## 7: case 0x ## x ## 8: case 0x ## x ## 9: case 0x ## x ## A: case 0x ## x ## B: \
case 0x ## x ## C: case 0x ## x ## D: case 0x ## x ## E: case 0x ## x ## F

u8* Memory::getMemoryAddress(FunkyBoy::memory_address offset) {
    switch ((offset >> 8) & 0xff) {
        FB_MEMORY_NIBBLE_RANGE(0):
        FB_MEMORY_NIBBLE_RANGE(1):
        FB_MEMORY_NIBBLE_RANGE(2):
        FB_MEMORY_NIBBLE_RANGE(3):
        FB_MEMORY_NIBBLE_RANGE(4):
        FB_MEMORY_NIBBLE_RANGE(5):
        FB_MEMORY_NIBBLE_RANGE(6):
        FB_MEMORY_NIBBLE_RANGE(7):
            return cartridge->mbc->getROMMemoryAddress(offset, cartridge->rom);
        FB_MEMORY_NIBBLE_RANGE(8):
        FB_MEMORY_NIBBLE_RANGE(9):
            return &ppuMemory.getVRAMByte(offset - 0x8000);
        FB_MEMORY_NIBBLE_RANGE(A):
        FB_MEMORY_NIBBLE_RANGE(B):
            return cartridge->mbc->getRAMMemoryAddress(offset - 0xA000, cartridge->ram);
        FB_MEMORY_NIBBLE_RANGE(C):
            return internalRam + (offset - 0xC000);
        FB_MEMORY_NIBBLE_RANGE(D):
            // TODO: Make this switchable
            return dynamicRamBank + (offset - 0xD000);
        FB_MEMORY_NIBBLE_RANGE(E):
            return internalRam + (offset - 0xE000);
        case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6:
        case 0xF7: case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
            return dynamicRamBank + (offset - 0xF000);
        case 0xFE: {
            if (offset < 0xFEA0) {
                return &ppuMemory.getOAMByte(offset - 0xFE00);
            } else {
                // Not usable
                return nullptr;
            }
        }
        case 0xFF: {
            if (offset == FB_REG_IE) {
                return &interruptEnableRegister;
            } else if (offset >= 0xFF80) {
                return hram + (offset - 0xFF80);
            } else {
                // Handled by interceptWrite and interceptReadAt
                fprintf(stderr, "Attempting to get access to HWIO, this means that the developer has forked something up\n");
                return nullptr;
            }
        }
        default:
            fprintf(stderr, "Attempt to access unmapped memory at 0x%04X\n", offset);
            return nullptr;
    }
}

u8 Memory::read8BitsAt(memory_address offset) {
    u8 data;
    if (!interceptReadAt(offset, &data)) {
#if defined(FB_DEBUG)
        fprintf(stderr, "Illegal 8-bit read from 0x%04X\n", offset);
#endif
        return 0;
    }
    return data;
}

i8 Memory::readSigned8BitsAt(memory_address offset) {
    u8 data;
    if (!interceptReadAt(offset, &data)) {
#if defined(FB_DEBUG)
        fprintf(stderr, "Illegal 8-bit read from 0x%04X\n", offset);
#endif
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
#if defined(FB_DEBUG)
        fprintf(stderr, "Illegal 8-bit write to 0x%04X => 0x%02X\n", offset, val);
#endif
        return;
    }
    *ptr = val;
}

u16 Memory::read16BitsAt(memory_address offset) {
    u8 lsb, msb;
    if (!interceptReadAt(offset, &lsb) || !interceptReadAt(offset + 1, &msb)) {
#if defined(FB_DEBUG)
        fprintf(stderr, "Illegal 16-bit read from 0x%04X\n", offset);
#endif
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
#if defined(FB_DEBUG)
        fprintf(stderr, "Illegal 16-bit write to 0x%04X\n", offset);
#endif
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
    ppuMemory.getOAMByte(dmaLsb) = read8BitsAt(Util::compose16Bits(dmaLsb, dmaMsb));
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