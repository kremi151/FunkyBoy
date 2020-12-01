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

#ifndef FB_CORE_UTIL_REGISTERS_H
#define FB_CORE_UTIL_REGISTERS_H

#include <util/debug.h>
#include <util/typedefs.h>

namespace FunkyBoy::Util {

    [[deprecated]]
    u16 addToSP(u8 *flags, u16 stackPointer, i8 val);

    u16_fast addToSPFast(u8_fast &flags, u16_fast stackPointer, i8_fast val);

    inline u8 *decodeRRAddressFromOpcode(u8 *registers, u8 opcode) {
        // Rightshift 4 seems to be universal when referencing 16bit registers in opcodes
        return registers + (((opcode >> 4) & 3) * 2); // 16-bit -> x 2
    }

}

#endif //FB_CORE_UTIL_REGISTERS_H
