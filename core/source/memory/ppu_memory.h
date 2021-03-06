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

#ifndef FB_CORE_MEMORY_PPU_MEMORY_H
#define FB_CORE_MEMORY_PPU_MEMORY_H

#include <util/typedefs.h>
#include <util/endianness.h>

#include <iostream>

namespace FunkyBoy {

    class PPUMemory {
    private:
        u8 *vram;
        u8 *oam;
        bool *vramAccessible;
        bool *oamAccessible;
        u16 *ptrCounter;
    public:
        PPUMemory();
        PPUMemory(const PPUMemory &other);
        ~PPUMemory();

        PPUMemory &operator=(const PPUMemory &other) = delete;

        inline u8 &getVRAMByte(memory_address vramOffset) {
            return *(vram + vramOffset);
        }

        inline u16 readVRAM16Bits(memory_address vramOffset) {
            return Util::compose16Bits(*(vram + vramOffset), *(vram + vramOffset + 1));
        }

        [[nodiscard]] inline bool isVRAMAccessibleFromMMU() const {
            return *vramAccessible;
        }

        inline u8 &getOAMByte(memory_address oamOffset) {
            return *(oam + oamOffset);
        }

        [[nodiscard]] inline bool isOAMAccessibleFromMMU() const {
            return *oamAccessible;
        }

        void setAccessibilityFromMMU(bool accessVram, bool accessOam);

        void serialize(std::ostream &ostream) const;
        void deserialize(std::istream &istream);
    };

}

#endif //FB_CORE_MEMORY_PPU_MEMORY_H
