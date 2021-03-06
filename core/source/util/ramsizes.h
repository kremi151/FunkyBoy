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

#ifndef FUNKYBOY_CORE_RAMSIZES_H
#define FUNKYBOY_CORE_RAMSIZES_H

#include <cstddef>

namespace FunkyBoy {

    enum RAMSize {
        RAM_SIZE_None = 0x0,
        RAM_SIZE_2KB = 0x1,
        RAM_SIZE_8KB = 0x2,
        RAM_SIZE_32KB = 0x3,
        RAM_SIZE_128KB = 0x4,
        RAM_SIZE_64KB = 0x5
    };

    inline size_t getRAMSizeInBytes(RAMSize ramSize) {
        switch (ramSize) {
            case RAMSize::RAM_SIZE_None:
                return 0;
            case RAMSize::RAM_SIZE_2KB:
                return 2048;
            case RAMSize::RAM_SIZE_8KB:
                return 8192;
            case RAMSize::RAM_SIZE_32KB:
                return 32768;
            case RAMSize::RAM_SIZE_64KB:
                return 65536;
            case RAMSize::RAM_SIZE_128KB:
                return 131072;
        }
    }

}

#endif //FUNKYBOY_CORE_RAMSIZES_H
