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

using namespace FunkyBoy;

#define FB_INTERNAL_RAM_BANK_SIZE (4 * 1024)
#define FB_ROM_BANK_SIZE (16 * 1024)

Memory::Memory(std::shared_ptr<Cartridge> cartridge): cartridge(std::move(cartridge)), interruptEnableFlag(0), romBank(1) {
    restartInterruptVectorTable = new u8[256]{};
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
    delete[] restartInterruptVectorTable;
    delete[] vram;
    delete[] bgMapData1;
    delete[] bgMapData2;
    delete[] internalRam;
    delete[] oam;
    delete[] hwIO;
    delete[] hram;
}

u8* Memory::getMemoryAddress(FunkyBoy::memory_address offset) {
    if (offset <= 0x00FF) {
        return restartInterruptVectorTable + offset;
    } else if (offset <= 0x3FFF) {
        return cartridge->rom + offset;
    } else if (offset <= 0x7FFF) {
        return cartridge->rom + (romBank * FB_ROM_BANK_SIZE) + (offset - 0x4000);
    } else if (offset <= 0x97FF) {
        return vram + (offset - 0x8000);
    } else if (offset <= 0x9BFF) {
        return bgMapData1 + (offset - 0x9800);
    } else if (offset <= 0x9FFF) {
        return bgMapData2 + (offset - 0x9C00);
    } else if (offset <= 0xBFFF) {
        return cartridge->ram + (offset - 0xA000);
    } else if (offset <= 0xCFFF) {
        return internalRam + (offset - 0xC000);
    } else if (offset <= 0xDFFF) {
        // TODO: Make this switchable
        return dynamicRamBank + (offset - 0xD000);
    } else if (offset <= 0xFDFF) {
        return nullptr;
    } else if (offset <= 0xFE9F) {
        return oam + (offset - 0xFE00);
    } else if (offset <= 0xFEFF) {
        return nullptr;
    } else if (offset <= 0xFF7F) {
        return hwIO + (offset - 0xFF00);
    } else if (offset <= 0xFFFE) {
        return hram + (offset - 0xFF80);
    } else if (offset == FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER) {
        return &interruptEnableFlag;
    } else {
        return nullptr;
    }
}

u8 Memory::read8BitsAt(memory_address offset) {
    return *getMemoryAddress(offset);
}

void Memory::write8BitsTo(memory_address offset, u8 val) {
    if (offset >= 0x0100 && offset <= 0x7FFF) {
        // Writing to ROM, meaning a ROM bank switch was requested
        romBank = val & 7;
        std::cout << "Switch ROM bank to " << (romBank & 0xff) << std::endl;
        return;
    }
    // TODO: Enable ram, switch ram bank, disable ram
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return;
    }
    *ptr = val;
}

void Memory::incrementAt(memory_address offset) {
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return;
    }
    (*ptr)++;
}

void Memory::decrementAt(memory_address offset) {
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return;
    }
    (*ptr)--;
}

u16 Memory::read16BitsAt(memory_address offset) {
    return *FB_CAST_8_TO_16_BIT(getMemoryAddress(offset));
}

void Memory::write16BitsTo(memory_address offset, u16 val) {
    if (offset >= 0x0100 && offset <= 0x7FFF) {
        // Writing to ROM, meaning a ROM bank switch was requested
        romBank = val & 7;
        std::cout << "Switch ROM bank to " << (romBank & 0xff) << std::endl;
        return;
    }
    // TODO: Enable ram, switch ram bank, disable ram
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return;
    }
    *FB_CAST_8_TO_16_BIT(getMemoryAddress(offset)) = val;
}

void Memory::write16BitsTo(memory_address offset, u8 msb, u8 lsb) {
    if (offset >= 0x0100 && offset <= 0x7FFF) {
        // Writing to ROM, meaning a ROM bank switch was requested
        romBank = lsb & 7;
        std::cout << "Switch ROM bank to " << (romBank & 0xff) << std::endl;
        return;
    }
    // TODO: Enable ram, switch ram bank, disable ram
    auto ptr = getMemoryAddress(offset);
    if (ptr == nullptr) {
        return;
    }
    *ptr = lsb;
    *(ptr+1) = msb;
}