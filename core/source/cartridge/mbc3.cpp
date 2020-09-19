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

#include "mbc3.h"

#include <util/debug.h>
#include <exception/state_exception.h>
#include <algorithm>
#include <cartridge/mbc1.h>

#define mbc3_print(...) debug_print_4(__VA_ARGS__)

#define FB_MBC3_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

MBC3::MBC3(ROMSize romSize, RAMSize ramSize, bool battery, bool rtc)
    : preliminaryRomBank(1)
    , ramBankSize(MBC1::getRAMBankSize(ramSize))
    , ramBankCount(MBC1::getRAMBankCount(ramSize))
    , maxRamOffset(MBC1::getMaxRAMOffset(ramSize))
#ifndef FB_IS_MBC3
    , ramBankingMode(false)
#endif
    , romSize(romSize)
    , ramEnabled(false)
    , useBattery(battery)
    , useRtc(rtc)
{
    updateBanks();
}

void MBC3::updateBanks() {
    mbc3_print("[MBC3] bank mode %d update banks from [rom=0x%02X,ram=0x%02X] to", ramBankingMode, romBank, ramBank);

    romBank = preliminaryRomBank & 0b1111111u;
#ifndef FB_IS_MBC3
    if (ramBankingMode) {
        romBank &= 0b11111;
        romBank |= (ramBank & 0b11) << 5;
#endif
        if (ramBank >= ramBankCount) {
            ramBank = std::max(0, ramBankCount - 1);
        }
#ifndef FB_IS_MBC3
    } else {
        ramBank = 0;
    }
#endif

    u8 romBankMask = MBC1::getROMBankBitMask(romSize);
    romBank &= romBankMask;

    romBankMask &= 0b1100000u;
    romBankOffsetLower = (preliminaryRomBank & romBankMask) * FB_MBC3_ROM_BANK_SIZE;
    romBankOffset = romBank * FB_MBC3_ROM_BANK_SIZE;
    ramBankOffset = ramBank * ramBankSize;

    mbc3_print(" [rom=0x%02X,ram=0x%02X]\n", romBank, ramBank);
}

u8 MBC3::readFromROMAt(memory_address offset, u8 *rom) {
    if (offset <= 0x3FFF) {
#ifndef FB_IS_MBC3
        if (ramBankingMode) {
            // TODO: Is this correct? Apparently it should be done like this according to https://gekkio.fi/files/gb-docs/gbctr.pdf
            return *(rom + romBankOffsetLower + offset);
        } else {
#endif
            return *(rom + offset);
#ifndef FB_IS_MBC3
        }
#endif
    } else if (offset <= 0x7FFF) {
        return *(rom + romBankOffset + (offset - 0x4000));
    } else {
        // Not readable
        return 0xff;
    }
}

void MBC3::interceptROMWrite(memory_address offset, u8 val) {
    if (offset <= 0x1FFF) {
        ramEnabled = ((val & 0xfu) == 0xA);
        mbc3_print("[MBC3] Enable RAM? %d\n", ramEnabled);
    } else if (offset <= 0x3FFF) {
        // Set ROM Bank number (lower 5 bits)
        val &= 0b0011111u;
#ifndef FB_IS_MBC3
        if (val == 0) val = 1;
#else
        if (val == 0 && !(romBank & 0b1100000u)) {
            val = 1;
        }
#endif
        mbc3_print("[MBC3] about to update ROM bank with value %d\n", val);
#ifndef FB_IS_MBC3
        preliminaryRomBank = (preliminaryRomBank & 0b1100000u) | val;
#else
        preliminaryRomBank = val & 0b1111111u;
#endif
        updateBanks();
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number or ROM Bank number (upper 2 bits)
        mbc3_print("[MBC3] about to update ROM/RAM bank with value %d\n", val);
#ifndef FB_IS_MBC3
        preliminaryRomBank = ((val & 0b11u) << 5) | (preliminaryRomBank & 0b0011111u);
        ramBank = val & 0b11u;
#else
        ramBank = val & 0b1111u;
#endif
        updateBanks();
#ifndef FB_IS_MBC3
    } else if (offset <= 0x7FFF) {
        ramBankingMode = val & 0b1;
        mbc3_print("[MBC3] Set banking mode to %d\n", ramBankingMode);
        updateBanks();
#endif
    }
}

u8 MBC3::readFromRAMAt(memory_address offset, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        // Not readable
        return 0xff;
    }
#ifdef FB_IS_MBC3
    switch (ramBank) {
        case 0x0: case 0x1: case 0x2: case 0x3: {
            return *(ram + ramBankOffset + offset);
        }
        case 0x8: {
            if (useRtc) {
                return rtc.getSeconds();
            } else {
                return 0xff;
            }
        }
        case 0x9: {
            if (useRtc) {
                return rtc.getMinutes();
            } else {
                return 0xff;
            }
        }
        case 0xA: {
            if (useRtc) {
                return rtc.getHours();
            } else {
                return 0xff;
            }
        }
        case 0xB: {
            if (useRtc) {
                return rtc.getDL();
            } else {
                return 0xff;
            }
        }
        case 0xC: {
            if (useRtc) {
                return rtc.getDH();
            } else {
                return 0xff;
            }
        }
        default: {
            // Not readable
            return 0xff;
        }
    }
#endif
}

void MBC3::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
#ifndef FB_IS_MBC3
    if (ramEnabled && offset <= maxRamOffset) {
        *(ram + ramBankOffset + offset) = val;
    }
#else
    if (!ramEnabled || offset > maxRamOffset) {
        // Not writable
        return;
    }
    switch (ramBank) {
        case 0x0: case 0x1: case 0x2: case 0x3: {
            *(ram + ramBankOffset + offset) = val;
        }
        case 0x8: {
            if (useRtc) {
                rtc.setSeconds(val);
            }
            break;
        }
        case 0x9: {
            if (useRtc) {
                rtc.setMinutes(val);
            }
            break;
        }
        case 0xA: {
            if (useRtc) {
                rtc.setHours(val);
            }
            break;
        }
        case 0xB: {
            if (useRtc) {
                rtc.setDL(val);
            }
            break;
        }
        case 0xC: {
            if (useRtc) {
                rtc.setDH(val);
            }
            break;
        }
    }
#endif
}

void MBC3::saveBattery(std::ostream &stream, u8 *ram, size_t l) {
    stream.write(static_cast<char*>(static_cast<void*>(ram)), l);
    if (useRtc) {
        rtc.write(stream);
    }
}

void MBC3::loadBattery(std::istream &stream, u8 *ram, size_t l) {
    stream.read(static_cast<char*>(static_cast<void*>(ram)), l);
    if (useRtc) {
        rtc.load(stream);
    }
}

bool MBC3::hasBattery() {
    return useBattery;
}