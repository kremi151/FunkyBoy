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

    void write16BitRegister(u8 *registers, u8 position, u16 val);
    u16 read16BitRegister(u8 *registers, u8 position);

    u16 composeHL(u8 &h, u8 &l);
    void writeHL(u8 &h, u8 &l, u16 val);

    u16 addToSP(u8 *flags, u16 stackPointer, i8 val);

}

#endif //FB_CORE_UTIL_REGISTERS_H