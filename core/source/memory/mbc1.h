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

namespace FunkyBoy {

    enum MBC1RAMSize {
        MBC1_NoRam = 0,
        MBC1_2KByte = 2048,
        MBC1_8KByte = 8192,
        MBC1_32KByte = 32768
    };

    class MBC1: public MBC {
    private:
        const MBC1RAMSize ramSize;
        u8 romBank;
        u8 ramBank;
        bool ramEnabled;
    public:
        MBC1(MBC1RAMSize ramSize);

        virtual u8 *getROMMemoryAddress(memory_address offset, u8 *rom) override;
        virtual u8 *getRAMMemoryAddress(memory_address offset, u8 *ram) override;
        virtual bool interceptWrite(memory_address offset, u8 val) override;
    };

}

#endif //FB_CORE_MBC1_H
