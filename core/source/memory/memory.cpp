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

#include <util/typedefs.h>
#include <util/debug.h>
#include <emulator/io_registers.h>

using namespace FunkyBoy;

#define FB_INTERNAL_RAM_BANK_SIZE (4 * 1024)

Memory::Memory(std::shared_ptr<Cartridge> cartridge, Controller::ControllersPtr controllers, io_registers_ptr ioRegisters): cartridge(std::move(cartridge))
    , controllers(std::move(controllers)), ioRegisters(std::move(ioRegisters)), interruptEnableRegister(0)
{
    vram = new u8[6144]{};
    bgMapData1 = new u8[1024]{};
    bgMapData2 = new u8[1024]{};
    internalRam = new u8[8 * FB_INTERNAL_RAM_BANK_SIZE]{};
    oam = new u8[160]{};
    hwIO = new u8[128]{};
    hram = new u8[127]{};

    dynamicRamBank = internalRam + FB_INTERNAL_RAM_BANK_SIZE;
}

Memory::~Memory() {
    delete[] vram;
    delete[] bgMapData1;
    delete[] bgMapData2;
    delete[] internalRam;
    delete[] oam;
    delete[] hwIO;
    delete[] hram;
}

u8* Memory::getMemoryAddress(FunkyBoy::memory_address offset) {
    if (offset <= 0x7FFF) {
        return cartridge->mbc->getROMMemoryAddress(offset, cartridge->rom);
    } else if (offset <= 0x97FF) {
        return vram + (offset - 0x8000);
    } else if (offset <= 0x9BFF) {
        return bgMapData1 + (offset - 0x9800);
    } else if (offset <= 0x9FFF) {
        return bgMapData2 + (offset - 0x9C00);
    } else if (offset <= 0xBFFF) {
        return cartridge->mbc->getRAMMemoryAddress(offset, cartridge->ram);
    } else if (offset <= 0xCFFF) {
        return internalRam + (offset - 0xC000);
    } else if (offset <= 0xDFFF) {
        // TODO: Make this switchable
        return dynamicRamBank + (offset - 0xD000);
    } else if (offset <= 0xFDFF) {
        // Echo RAM
        // TODO: Verify that this offset is correctly calculated (Echo RAM offset + boundary of RAM bank 0)
        if (offset <= 0xEFFF) {
            return internalRam + (offset - 0xE000);
        } else {
            return dynamicRamBank + (offset - 0xF000);
        }
    } else if (offset <= 0xFE9F) {
        return oam + (offset - 0xFE00);
    } else if (offset <= 0xFEFF) {
        return nullptr;
    } else if (offset == FB_REG_DIV) {
        // Write access is protected by interceptWrite
        return const_cast<u8 *>(&ioRegisters->sysCounterMsb());
    } else if (offset <= 0xFF7F) {
        return hwIO + (offset - 0xFF00);
    } else if (offset <= 0xFFFE) {
        return hram + (offset - 0xFF80);
    } else if (offset == FB_REG_IE) {
        return &interruptEnableRegister;
    } else {
        return nullptr;
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

bool Memory::interceptWrite(FunkyBoy::memory_address offset, FunkyBoy::u8 val) {
    if (offset <= 0x7FFF && cartridge->mbc->interceptWrite(offset, val)) {
        // Writing to read-only area, so we let it intercept by the MBC
        return true;
    }
    if (offset == FB_REG_SC && val == 0x81) {
        controllers->getSerial()->sendByte(read8BitsAt(FB_REG_SB));
    }
    if (offset == FB_REG_DIV) {
        // Direct write to DIV ; reset to 0
        ioRegisters->resetSysCounter();
        return true;
    }
    return false;
}

bool Memory::interceptReadAt(FunkyBoy::memory_address offset, u8 *out) {
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return false;
    }
    *out = *ptr;
    return true;
}

u8 Memory::updateJoypad() {
    u8 *p1 = getMemoryAddress(FB_REG_P1);
    u8 originalValue = *p1;
    u8 val = originalValue | 0b11001111u;
    auto &joypad = *controllers->getJoypad();
    if ((originalValue & 0b00100000u) == 0) {
        // Select Button keys
        if (joypad.isKeyPressed(Controller::JOYPAD_A)) {
            val &= 0b11111110u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_B)) {
            val &= 0b11111101u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_SELECT)) {
            val &= 0b11111011u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_START)) {
            val &= 0b11110111u;
        }
    }
    if ((originalValue & 0b00010000u) == 0) {
        // Select Direction keys
        if (joypad.isKeyPressed(Controller::JOYPAD_RIGHT)) {
            val &= 0b11111110u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_LEFT)) {
            val &= 0b11111101u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_UP)) {
            val &= 0b11111011u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_DOWN)) {
            val &= 0b11110111u;
        }
    }
    *p1 = val;
    return val;
}

void Memory::write8BitsTo(memory_address offset, u8 val) {
    if (interceptWrite(offset, val)) {
        return;
    }
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        fprintf(stderr, "Illegal 8-bit write to 0x%04X\n", offset);
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

#ifdef FB_TESTING

io_registers_ptr& Memory::getIoRegisters() {
    return ioRegisters;
}

CartridgePtr& Memory::getCartridge() {
    return cartridge;
}

#endif