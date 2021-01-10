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
#include <util/string_polyfills.h>

#define mbc3_print(...) debug_print_4(__VA_ARGS__)

#define FB_MBC3_ROM_BANK_SIZE (16 * 1024)

using namespace FunkyBoy;

size_t MBC3::getRAMBankSize(RAMSize size) {
    switch (size) {
        case RAMSize::RAM_SIZE_None:
            return 0;
        case RAMSize::RAM_SIZE_2KB:
            return getRAMSizeInBytes(RAMSize::RAM_SIZE_2KB);
        case RAMSize::RAM_SIZE_8KB:
        case RAMSize::RAM_SIZE_32KB:
        case RAMSize::RAM_SIZE_64KB:
            return getRAMSizeInBytes(RAMSize::RAM_SIZE_8KB);
        default:
            throw Exception::WrongStateException("Invalid MBC3 RAM size: " + Util::toString(size));
    }
}

u8 MBC3::getRAMBankCount(RAMSize size) {
    switch (size) {
        case RAMSize::RAM_SIZE_None:
            return 0;
        case RAMSize::RAM_SIZE_2KB:
        case RAMSize::RAM_SIZE_8KB:
            return 1;
        case RAMSize::RAM_SIZE_32KB:
            return 4;
        case RAMSize::RAM_SIZE_64KB:
            return 8;
        default:
            throw Exception::WrongStateException("Invalid MBC3 RAM size: " + Util::toString(size));
    }
}

memory_address MBC3::getMaxRAMOffset(RAMSize ramSize) {
    switch (ramSize) {
        case RAMSize::RAM_SIZE_None:
            return 0x0000;
        case RAMSize::RAM_SIZE_2KB:
            return 0x07FF;
        case RAMSize::RAM_SIZE_8KB:
        case RAMSize::RAM_SIZE_32KB:
        case RAMSize::RAM_SIZE_64KB:
            return 0x1FFF;
        default:
            throw Exception::WrongStateException("Invalid MBC3 RAM size: " + Util::toString(ramSize));
    }
}

MBC3::MBC3(ROMSize romSize, RAMSize ramSize, bool battery, bool rtc, bool mbc30)
    : preliminaryRomBank(1)
    , ramBankSize(getRAMBankSize(ramSize))
    , ramBankCount(getRAMBankCount(ramSize))
    , maxRamOffset(getMaxRAMOffset(ramSize))
    , romSize(romSize)
    , ramEnabled(false)
    , useBattery(battery)
    , useRtc(rtc)
    , romBankMask(mbc30 ? 0b11111111u : 0b1111111u)
    , ramBankMask(mbc30 ? 0b111u : 0b11u)
{
    updateBanks();
}

void MBC3::updateBanks() {
    mbc3_print("[MBC3] update banks from [rom=0x%02X,ram=0x%02X] to", romBank, ramBank);

    romBank = preliminaryRomBank & 0b1111111u;
    if (ramBank < 0x8 || ramBank > 0xC) {
        ramBank &= ramBankMask;
        if (ramBank >= ramBankCount) {
            ramBank = std::max(0, ramBankCount - 1);
        }
    }

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
        return *(rom + offset);
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
        if (val == 0 && !(romBank & 0b1100000u)) {
            val = 1;
        }
        mbc3_print("[MBC3] about to update ROM bank with value %d\n", val);
        preliminaryRomBank = val & romBankMask;
        updateBanks();
    } else if (offset <= 0x5FFF) {
        // Set RAM Bank number or ROM Bank number (upper 2 bits)
        mbc3_print("[MBC3] about to update ROM/RAM bank with value %d\n", val);
        ramBank = val;
        updateBanks();
    }
}

u8 MBC3::readFromRAMAt(memory_address offset, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        // Not readable
        return 0xff;
    }
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
}

bool MBC3::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
    if (!ramEnabled || offset > maxRamOffset) {
        // Not writable
        return false;
    }
    switch (ramBank) {
        case 0x0: case 0x1: case 0x2: case 0x3: {
            *(ram + ramBankOffset + offset) = val;
            break;
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
    return true;
}

void MBC3::saveBattery(std::ostream &stream, u8 *ram, size_t l) {
    stream.write(reinterpret_cast<char*>(ram), l);
    if (useRtc) {
        rtc.write(stream);
    }
}

void MBC3::loadBattery(std::istream &stream, u8 *ram, size_t l) {
    stream.read(reinterpret_cast<char*>(ram), l);
    if (useRtc) {
        rtc.load(stream);
    }
}

bool MBC3::hasBattery() {
    return useBattery;
}

void MBC3::getDebugInfo(const char **outName, unsigned int &outRomBank) {
    *outName = "MBC3";
    outRomBank = romBank;
}