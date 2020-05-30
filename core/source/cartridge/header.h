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

#ifndef FUNKYBOY_CORE_CARTRIDGE_HEADER_H
#define FUNKYBOY_CORE_CARTRIDGE_HEADER_H

#include <util/typedefs.h>

#define FB_ROM_HEADER_ENTRY_POINT 0x100

namespace FunkyBoy {

#pragma pack(push, 1)

    struct ROMHeader {
        u8 unknown_1[FB_ROM_HEADER_ENTRY_POINT];    // 0x0000 - 0x0099

        u8 entryPoint[4];                           // 0x0100 - 0x0103
        u8 nintendoLogo[48];                        // 0x0104 - 0x0133
        u8 title[15];                               // 0x0134 - 0x0142
        u8 cgbFlag;                                 // 0x0143
        u8 newLicenseCode[2];                       // 0x0144 - 0x0145
        u8 sgbFlag;                                 // 0x0146
        u8 cartridgeType;                           // 0x0147
        u8 romSize;                                 // 0x0148
        u8 ramSize;                                 // 0x0149
        u8 destinationCode;                         // 0x014A
        u8 oldLicenseCode;                          // 0x014B
        u8 maskRomVersionNumber;                    // 0x014C
        u8 headerChecksum;                          // 0x014D
        u8 globalChecksum[2];                       // 0x014E - 0x014F
    };

#pragma pack(pop)

}

#endif //FUNKYBOY_CORE_CARTRIDGE_HEADER_H
