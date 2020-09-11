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

#include "mbc2.h"

#include <util/debug.h>
#include <exception/state_exception.h>

#define mbc2_print(...) fprintf(stdout, __VA_ARGS__)

#define FB_MBC2_ROM_BANK_SIZE (16 * 1024)
#define FB_MBC2_MAX_RAM_OFFSET (0xA1FF - 0xA000)

using namespace FunkyBoy;

u8 __fb_mbc2_getROMBankBitMask(ROMSize romSize) {
    switch (romSize) {
        case ROMSize::ROM_SIZE_32K:
            return 0b1u;
        case ROMSize::ROM_SIZE_64K:
            return 0b11u;
        case ROMSize::ROM_SIZE_128K:
            return 0b111u;
        case ROMSize::ROM_SIZE_256K:
        case ROMSize::ROM_SIZE_512K:
        case ROMSize::ROM_SIZE_1024K:
        case ROMSize::ROM_SIZE_2048K:
        case ROMSize::ROM_SIZE_4096K:
            return 0b1111u;
        default:
            mbc2_print("Invalid ROM size for MBC2 chip: %d\n", romSize);
            throw Exception::WrongStateException(&"Invalid ROM size for MBC2 chip: " [ romSize]);
    }
}

MBC2::MBC2(ROMSize romSize)
    : romSize(romSize)
    , ramEnabled(false)
    , romBank(1)
    , romBankOffset(1 * FB_MBC2_ROM_BANK_SIZE)
{
}

u8 MBC2::readFromROMAt(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
        return *(rom + offset);
    } else if (offset <= 0x7FFF) {
        return *(rom + romBankOffset + (offset - 0x4000));
    } else {
        // Not readable
        return 0xff;
    }
}

void MBC2::interceptROMWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = ((val & 0xfu) == 0xA);
        mbc2_print("[MBC2] Enable RAM? %d\n", ramEnabled);
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number
        val &= 0b1111u;
        if (val == 0) val = 1;
        romBank = val & __fb_mbc2_getROMBankBitMask(romSize);
        romBankOffset = romBank * FB_MBC2_ROM_BANK_SIZE;
        mbc2_print("[MBC2] Select ROM bank 0x%02X\n", romBank);
    }
}

u8 MBC2::readFromRAMAt(memory_address offset, u8 *ram) {
    if (!ramEnabled) {
        // Not readable
        return 0xff;
    }
    // When going higher than 0xA1FF, the RAM just wraps around (i.e. starts reading again from 0xA000)
    return (*(ram + (offset % (FB_MBC2_MAX_RAM_OFFSET + 1))) & 0b1111u) | 0b11110000u;
}

void MBC2::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
    if (ramEnabled && offset <= FB_MBC2_MAX_RAM_OFFSET) {
        *(ram + offset) = val & 0b1111u;
    }
}