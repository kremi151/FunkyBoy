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

#include <cstdio>
#include <util/debug.h>
#include "mbc1.h"

#define FB_MBC1_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

MBC1::MBC1(MBC1RAMSize ramSize): bank(1), romBankingMode(true), ramSize(ramSize), ramEnabled(false) {
}

inline u8 MBC1::getROMBank() {
    return romBankingMode ? (bank & 0b1111111) : (bank & 0b11111);
}

inline u8 MBC1::getRAMBank() {
    return romBankingMode ? 0 : ((bank >> 5) & 0b11);
}

u8 * MBC1::getROMMemoryAddress(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
        return rom + offset;
    } else if (offset <= 0x7FFF) {
        return rom + (getROMBank() * FB_MBC1_ROM_BANK_SIZE) + (offset - 0x4000);
    } else {
        return nullptr;
    }
}

u8 getMBC1RAMBankSize(MBC1RAMSize size) {
    switch (size) {
        case MBC1_2KByte:
            return static_cast<u8>(MBC1_2KByte);
        case MBC1_8KByte:
        case MBC1_32KByte:
            return static_cast<u8>(MBC1_8KByte);
        default:
            return 0;
    }
}

u8 * MBC1::getRAMMemoryAddress(memory_address offset, u8 *ram) {
    if (!ramEnabled) {
        return nullptr;
    } else if (ramSize == MBC1RAMSize::MBC1_NoRam || offset < 0xA000) {
        return nullptr;
    } else if (ramSize == MBC1RAMSize::MBC1_2KByte) {
        if (offset > 0xA7FF) {
            return nullptr;
        }
    } else if (ramSize == MBC1RAMSize::MBC1_8KByte || ramSize == MBC1RAMSize::MBC1_32KByte) {
        if (offset > 0xBFFF) {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return ram + (getRAMBank() * getMBC1RAMBankSize(ramSize)) + (offset - 0xA000);
}

bool MBC1::interceptWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = (val == 0x0A);
        debug_print_2("[MBC1] Enable RAM? %d\n", ramEnabled);
        return true;
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number (lower 5 bits)
        if (val == 0) val = 1;
        debug_print_2("[MBC1] bank mode %d switch ROM bank from 0x%02X to", !romBankingMode, bank);
        bank = (bank & 0b1100000) | (val & 0b0011111);
        debug_print_2(" 0x%02X\n", bank);
        return true;
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number or ROM Bank number (upper 2 bits)
        debug_print_2("[MBC1] bank mode %d switch ROM/RAM bank from 0x%02X to", !romBankingMode, bank);
        bank = ((val & 0b11) << 5) | (bank & 0x0011111);
        debug_print_2(" 0x%02X\n", bank);
        return true;
    } else if (offset <= 0x7FFFF) {
        romBankingMode = (val & 0b1) == 0;
        debug_print_2("[MBC1] Set banking mode to %d\n", !romBankingMode);
        return true;
    }
    return false;
}
