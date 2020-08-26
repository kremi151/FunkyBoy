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

#include "mbc1.h"

#include <util/debug.h>
#include <exception/state_exception.h>
#include <algorithm>

#define mbc1_print(...) debug_print_4(__VA_ARGS__)

#define FB_MBC1_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

u16 getMBC1RAMBankSize(MBC1RAMSize size) {
    if (size == MBC1RAMSize::MBC1_NoRam) {
        return 0;
    } else if (size == MBC1RAMSize::MBC1_2KByte) {
        return static_cast<u16>(MBC1_2KByte);
    } else if (size == MBC1RAMSize::MBC1_8KByte || size == MBC1RAMSize::MBC1_32KByte) {
        return static_cast<u16>(MBC1_8KByte);
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + std::to_string(size));
    }
}

u8 getMBC1RAMBankCount(MBC1RAMSize size) {
    if (size == MBC1RAMSize::MBC1_NoRam) {
        return 0;
    } else if (size == MBC1RAMSize::MBC1_2KByte || size == MBC1RAMSize::MBC1_8KByte) {
        return 1;
    } else if (size == MBC1RAMSize::MBC1_32KByte) {
        return 4;
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + std::to_string(size));
    }
}

memory_address getMaxRAMOffset(MBC1RAMSize ramSize) {
    if (ramSize == MBC1RAMSize::MBC1_NoRam) {
        return 0x0000;
    } else if (ramSize == MBC1RAMSize::MBC1_2KByte) {
        return 0x07FF;
    } else if (ramSize == MBC1RAMSize::MBC1_8KByte || ramSize == MBC1RAMSize::MBC1_32KByte) {
        return 0x1FFF;
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + std::to_string(ramSize));
    }
}

u8 getROMBankBitMask(ROMSize romSize) {
    switch (romSize) {
        case ROMSize::ROM_SIZE_32K:
            // 32K ROMs have in theory no banking, as bank 0 is mapped to 0x0000-0x3FFF
            // and bank 1 to 0x4000-0x7000, with every bank being 16K
            return 0b1u;
        case ROMSize::ROM_SIZE_64K:
            return 0b11u;
        case ROMSize::ROM_SIZE_128K:
            return 0b111u;
        case ROMSize::ROM_SIZE_256K:
            return 0b1111u;
        case ROMSize::ROM_SIZE_512K:
            return 0b11111u;
        case ROMSize::ROM_SIZE_1024K:
            return 0b111111u;
        case ROMSize::ROM_SIZE_2048K:
        case ROMSize::ROM_SIZE_4096K:
            return 0b1111111u;
        default:
            return 0b0u;
    }
}

MBC1::MBC1(ROMSize romSize, MBC1RAMSize ramSize)
    : preliminaryRomBank(1)
    , ramBankSize(getMBC1RAMBankSize(ramSize))
    , ramBankCount(getMBC1RAMBankCount(ramSize))
    , maxRamOffset(getMaxRAMOffset(ramSize))
    , ramBankingMode(false)
    , romSize(romSize)
    , ramSize(ramSize)
    , ramEnabled(false)
{
    updateBanks();
}

void MBC1::updateBanks() {
    mbc1_print("[MBC1] bank mode %d update banks from [rom=0x%02X,ram=0x%02X] to", ramBankingMode, romBank, ramBank);

    romBank = preliminaryRomBank & 0b1111111u;
    if (ramBankingMode) {
        romBank &= 0b11111;
        romBank |= (ramBank & 0b11) << 5;
    }
    if (ramBank >= ramBankCount) {
        ramBank = std::max(0, ramBankCount - 1);
    }

    u8 romBankMask = getROMBankBitMask(romSize);
    romBank &= romBankMask;

    romBankMask &= 0b1100000u;
    romBankOffsetLower = (preliminaryRomBank & romBankMask) * FB_MBC1_ROM_BANK_SIZE;
    romBankOffset = romBank * FB_MBC1_ROM_BANK_SIZE;
    ramBankOffset = ramBank * ramBankSize;

    mbc1_print(" [rom=0x%02X,ram=0x%02X]\n", romBank, ramBank);
}

u8 * MBC1::getROMMemoryAddress(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
        if (ramBankingMode) {
            // TODO: Is this correct? Apparently it should be done like this according to https://gekkio.fi/files/gb-docs/gbctr.pdf
            return rom + romBankOffsetLower + offset;
        } else {
            return rom + offset;
        }
    } else if (offset <= 0x7FFF) {
        return rom + romBankOffset + (offset - 0x4000);
    } else {
        return nullptr;
    }
}

u8 * MBC1::getRAMMemoryAddress(memory_address offset, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        return nullptr;
    }
    return ram + ramBankOffset + offset;
}

bool MBC1::interceptWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = ((val & 0xfu) == 0xA);
        mbc1_print("[MBC1] Enable RAM? %d\n", ramEnabled);
        return true;
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number (lower 5 bits)
        val &= 0b0011111u;
        if (val == 0) val = 1;
        mbc1_print("[MBC1] about to update ROM bank with value %d\n", val);
        preliminaryRomBank = (preliminaryRomBank & 0b1100000u) | val;
        updateBanks();
        return true;
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number or ROM Bank number (upper 2 bits)
        mbc1_print("[MBC1] about to update ROM/RAM bank with value %d\n", val);
        preliminaryRomBank = ((val & 0b11u) << 5) | (preliminaryRomBank & 0b0011111u);
        ramBank = val & 0b11u;
        updateBanks();
        return true;
    } else if (offset <= 0x7FFF) {
        ramBankingMode = val & 0b1;
        mbc1_print("[MBC1] Set banking mode to %d\n", ramBankingMode);
        updateBanks();
        return true;
    }
    return false;
}
