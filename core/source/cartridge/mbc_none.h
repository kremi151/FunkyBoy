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

#ifndef FB_CORE_MBC_NONE_H
#define FB_CORE_MBC_NONE_H

#include <cartridge/mbc.h>

namespace FunkyBoy {

    class MBCNone: public MBC {
    public:
        u8 readFromROMAt(memory_address offset, u8 *rom) override;
        void interceptROMWrite(memory_address offset, u8 val) override;

        u8 readFromRAMAt(memory_address offset, u8 *ram) override;
        void writeToRAMAt(memory_address offset, u8 val, u8 *ram) override;

        void saveBattery(std::ostream &stream, u8 *ram, size_t l) override;
        void loadBattery(std::istream &stream, u8 *ram, size_t l) override;

        bool hasBattery() override;

        void getDebugInfo(const char **outName, unsigned &outRomBank) override;
    };

}

#endif //FB_CORE_MBC_NONE_H
