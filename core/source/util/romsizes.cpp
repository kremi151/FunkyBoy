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

#include "romsizes.h"

unsigned int FunkyBoy::romSizeInBytes(ROMSize size) {
    switch (size) {
        case ROM_SIZE_32K:
            return 32 * 1024;
        case ROM_SIZE_64K:
            return 64 * 1024;
        case ROM_SIZE_128K:
            return 128 * 1024;
        case ROM_SIZE_256K:
            return 256 * 1024;
        case ROM_SIZE_512K:
            return 512 * 1024;
        case ROM_SIZE_1M:
            return 1024 * 1024;
        case ROM_SIZE_2M:
            return 2048 * 1024;
        case ROM_SIZE_4M:
            return 4096 * 1024;
        case ROM_SIZE_8M:
            return 8 * 1024 * 1024;
        // TODO: Check compatibility and re-enable:
        // case ROM_SIZE_1_1M:
        //     return 72 * 16 * 1024;
        // case ROM_SIZE_1_2M:
        //    return 80 * 16 * 1024;
        // case ROM_SIZE_1_5M:
        //    return 96 * 16 * 1024;
        default:
            return 0;
    }
}