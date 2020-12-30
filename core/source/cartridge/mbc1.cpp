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
#include <util/string_polyfills.h>
#include <algorithm>

#define mbc1_print(...) debug_print_4(__VA_ARGS__)

#define FB_MBC1_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

size_t MBC1::getRAMBankSize(RAMSize size) {
    if (size == RAMSize::RAM_SIZE_None) {
        return 0;
    } else if (size == RAMSize::RAM_SIZE_2KB) {
        return getRAMSizeInBytes(RAMSize::RAM_SIZE_2KB);
    } else if (size == RAMSize::RAM_SIZE_8KB || size == RAMSize::RAM_SIZE_32KB) {
        return getRAMSizeInBytes(RAMSize::RAM_SIZE_8KB);
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + Util::toString(size));
    }
}

u8 MBC1::getRAMBankCount(RAMSize size) {
    if (size == RAMSize::RAM_SIZE_None) {
        return 0;
    } else if (size == RAMSize::RAM_SIZE_2KB || size == RAMSize::RAM_SIZE_8KB) {
        return 1;
    } else if (size == RAMSize::RAM_SIZE_32KB) {
        return 4;
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + Util::toString(size));
    }
}

memory_address MBC1::getMaxRAMOffset(RAMSize ramSize) {
    if (ramSize == RAMSize::RAM_SIZE_None) {
        return 0x0000;
    } else if (ramSize == RAMSize::RAM_SIZE_2KB) {
        return 0x07FF;
    } else if (ramSize == RAMSize::RAM_SIZE_8KB || ramSize == RAMSize::RAM_SIZE_32KB) {
        return 0x1FFF;
    } else {
        throw Exception::WrongStateException("Invalid MBC1 RAM size: " + Util::toString(ramSize));
    }
}

u8 MBC1::getROMBankBitMask(ROMSize romSize) {
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

MBC1::MBC1(ROMSize romSize, RAMSize ramSize, bool battery)
    : preliminaryRomBank(1)
    , ramBankSize(getRAMBankSize(ramSize))
    , ramBankCount(getRAMBankCount(ramSize))
    , maxRamOffset(getMaxRAMOffset(ramSize))
    , ramBankingMode(false)
    , romSize(romSize)
    , ramEnabled(false)
    , battery(battery)
{
    updateBanks();
}

void MBC1::updateBanks() {
    mbc1_print("[MBC1] bank mode %d update banks from [rom=0x%02X,ram=0x%02X] to", ramBankingMode, romBank, ramBank);

    romBank = preliminaryRomBank & 0b1111111u;
    if (ramBankingMode) {
        romBank &= 0b11111;
        romBank |= (ramBank & 0b11) << 5;

        if (ramBank >= ramBankCount) {
            ramBank = std::max(0, ramBankCount - 1);
        }
    } else {
        ramBank = 0;
    }

    u8 romBankMask = getROMBankBitMask(romSize);
    romBank &= romBankMask;

    romBankMask &= 0b1100000u;
    romBankOffsetLower = (preliminaryRomBank & romBankMask) * FB_MBC1_ROM_BANK_SIZE;
    romBankOffset = romBank * FB_MBC1_ROM_BANK_SIZE;
    ramBankOffset = ramBank * ramBankSize;

    mbc1_print(" [rom=0x%02X,ram=0x%02X]\n", romBank, ramBank);
}

u8 MBC1::readFromROMAt(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
        if (ramBankingMode) {
            // TODO: Is this correct? Apparently it should be done like this according to https://gekkio.fi/files/gb-docs/gbctr.pdf
            return *(rom + romBankOffsetLower + offset);
        } else {
            return *(rom + offset);
        }
    } else if (offset <= 0x7FFF) {
        return *(rom + romBankOffset + (offset - 0x4000));
    } else {
        // Not readable
        return 0xff;
    }
}

void MBC1::interceptROMWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = ((val & 0xfu) == 0xA);
        mbc1_print("[MBC1] Enable RAM? %d\n", ramEnabled);
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number (lower 5 bits)
        val &= 0b0011111u;
        if (val == 0) val = 1;
        mbc1_print("[MBC1] about to update ROM bank with value %d\n", val);
        preliminaryRomBank = (preliminaryRomBank & 0b1100000u) | val;
        updateBanks();
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number or ROM Bank number (upper 2 bits)
        mbc1_print("[MBC1] about to update ROM/RAM bank with value %d\n", val);
        preliminaryRomBank = ((val & 0b11u) << 5) | (preliminaryRomBank & 0b0011111u);
        ramBank = val & 0b11u;
        updateBanks();
    } else if (offset <= 0x7FFF) {
        ramBankingMode = val & 0b1;
        mbc1_print("[MBC1] Set banking mode to %d\n", ramBankingMode);
        updateBanks();
    }
}

u8 MBC1::readFromRAMAt(memory_address offset, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        // Not readable
        return 0xff;
    }
    return *(ram + ramBankOffset + offset);
}

void MBC1::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
    if (ramEnabled && offset <= maxRamOffset) {
        *(ram + ramBankOffset + offset) = val;
    }
}

void MBC1::saveBattery(std::ostream &stream, u8 *ram, size_t l) {
    stream.write(static_cast<char*>(static_cast<void*>(ram)), l);
}

void MBC1::loadBattery(std::istream &stream, u8 *ram, size_t l) {
    stream.read(static_cast<char*>(static_cast<void*>(ram)), l);
}

bool MBC1::hasBattery() {
    return battery;
}