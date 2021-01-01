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

#ifndef FUNKYBOY_CORE_ROMSIZES_H
#define FUNKYBOY_CORE_ROMSIZES_H

namespace FunkyBoy {

    enum ROMSize {
        ROM_SIZE_32K = 0,
        ROM_SIZE_64K = 1,
        ROM_SIZE_128K = 2,
        ROM_SIZE_256K = 3,
        ROM_SIZE_512K = 4,
        ROM_SIZE_1M = 5,
        ROM_SIZE_2M = 6,
        ROM_SIZE_4M = 7,
        ROM_SIZE_8M = 8,
        // TODO: Check compatibility and re-enable:
        // ROM_SIZE_1_1M = 0x52,
        // ROM_SIZE_1_2M = 0x53,
        // ROM_SIZE_1_5M = 0x54,
    };

    unsigned int romSizeInBytes(ROMSize size);

}

#endif //FUNKYBOY_CORE_ROMSIZES_H
