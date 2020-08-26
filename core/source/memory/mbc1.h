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

#ifndef FB_CORE_MBC1_H
#define FB_CORE_MBC1_H

#include <memory/mbc.h>
#include <util/testing.h>
#include <util/romsizes.h>

namespace FunkyBoy {

    enum MBC1RAMSize {
        MBC1_NoRam = 0,
        MBC1_2KByte = 2048,
        MBC1_8KByte = 8192,
        MBC1_32KByte = 32768
    };

    class MBC1: public MBC {
    private:
        const ROMSize romSize;
        const MBC1RAMSize ramSize;
        const u16 ramBankSize;
        const u8 ramBankCount;
        const memory_address maxRamOffset;
        u32 romBankOffsetLower{};
        u32 romBankOffset{};
        u32 ramBankOffset{};
        void updateBanks();
    test_public:
        u8 preliminaryRomBank, romBank{};
        u8 ramBank{};
        bool ramBankingMode;
        bool ramEnabled;
    public:
        MBC1(ROMSize romSize, MBC1RAMSize ramSize);

        u8 readFromROMAt(memory_address offset, u8 *rom) override;
        void interceptROMWrite(memory_address offset, u8 val) override;

        u8 readFromRAMAt(memory_address offset, u8 *ram) override;
        void writeToRAMAt(memory_address offset, u8 val, u8 *ram) override;
    };

}

#endif //FB_CORE_MBC1_H
