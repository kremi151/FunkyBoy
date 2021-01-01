/**
 * Copyright 2021 Michel Kremer (kremi151)
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

#include "mbc5.h"

#include <cartridge/mbc1.h>
#include <util/debug.h>
#include <exception/state_exception.h>
#include <util/string_polyfills.h>
#include <algorithm>

#define mbc5_print(...) debug_print_4(__VA_ARGS__)

#define FB_MBC5_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

MBC5::MBC5(ROMSize romSize, RAMSize ramSize, bool battery)
    : preliminaryRomBank(1)
    , ramBankSize(MBC1::getRAMBankSize(ramSize))
    , ramBankCount(MBC1::getRAMBankCount(ramSize))
    , maxRamOffset(MBC1::getMaxRAMOffset(ramSize))
    , romSize(romSize)
    , ramEnabled(false)
    , battery(battery)
{
    updateBanks();
}

void MBC5::updateBanks() {
    mbc5_print("[MBC5] bank mode %d update banks from [rom=0x%02X,ram=0x%02X] to", ramBankingMode, romBank, ramBank);

    romBank = preliminaryRomBank & 0b1111111u;
    if (ramBank >= ramBankCount) {
        ramBank = std::max(0, ramBankCount - 1);
    }

    u8 romBankMask = MBC1::getROMBankBitMask(romSize);
    romBank &= romBankMask;

    romBankOffset = romBank * FB_MBC5_ROM_BANK_SIZE;
    ramBankOffset = ramBank * ramBankSize;

    mbc5_print(" [rom=0x%02X,ram=0x%02X]\n", romBank, ramBank);
}

u8 MBC5::readFromROMAt(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
        return *(rom + offset);
    } else if (offset <= 0x7FFF) {
        return *(rom + romBankOffset + (offset - 0x4000));
    } else {
        // Not readable
        return 0xff;
    }
}

void MBC5::interceptROMWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = (val & 0xfu) == 0xA;
        mbc5_print("[MBC5] Enable RAM? %d\n", ramEnabled);
    } else if (offset <= 0x2FFF) {
        // Set ROM Bank number (lower 8 bits ; bits 0 - 7)
        mbc5_print("[MBC5] about to update ROM bank with lower bits %d\n", val);
        preliminaryRomBank = (preliminaryRomBank & 0b100000000u) | val;
        updateBanks();
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number (upper bit ; bit 8)
        mbc5_print("[MBC5] about to update ROM bank with upper bit %d\n", val);
        preliminaryRomBank = (preliminaryRomBank & 0b011111111u) | (val << 8u);
        updateBanks();
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number
        mbc5_print("[MBC5] about to update RAM bank with value %d\n", val);
        ramBank = val & 0b1111u;
        updateBanks();
    }
}

u8 MBC5::readFromRAMAt(memory_address offset, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        // Not readable
        return 0xff;
    }
    return *(ram + ramBankOffset + offset);
}

void MBC5::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
    if (ramEnabled && offset <= maxRamOffset) {
        *(ram + ramBankOffset + offset) = val;
    }
}

void MBC5::saveBattery(std::ostream &stream, u8 *ram, size_t l) {
    stream.write(static_cast<char*>(static_cast<void*>(ram)), l);
}

void MBC5::loadBattery(std::istream &stream, u8 *ram, size_t l) {
    stream.read(static_cast<char*>(static_cast<void*>(ram)), l);
}

bool MBC5::hasBattery() {
    return battery;
}

void MBC5::getDebugInfo(const char **outName, unsigned &outRomBank) {
    *outName = "MBC5";
    outRomBank = romBank;
}