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

#ifndef FB_CORE_MBC_H
#define FB_CORE_MBC_H

#include <util/typedefs.h>
#include <iostream>

namespace FunkyBoy {

    class MBC {
    public:
        virtual ~MBC() = default;

        virtual u8 readFromROMAt(memory_address offset, u8 *rom) = 0;
        virtual void interceptROMWrite(memory_address offset, u8 val) = 0;

        virtual u8 readFromRAMAt(memory_address offset, u8 *ram) = 0;
        virtual bool writeToRAMAt(memory_address offset, u8 val, u8 *ram) = 0;

        virtual void saveBattery(std::ostream &stream, u8 *ram, size_t l) = 0;
        virtual void loadBattery(std::istream &stream, u8 *ram, size_t l) = 0;

        virtual void serialize(std::ostream &ostream) const = 0;
        virtual void deserialize(std::istream &istream) = 0;

        virtual bool hasBattery() = 0;

        virtual void getDebugInfo(const char **outName, unsigned &outRomBank) = 0;
    };

}

#endif //FB_CORE_MBC_H
