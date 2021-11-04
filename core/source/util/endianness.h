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

#ifndef FB_CORE_ENDIANNESS_H
#define FB_CORE_ENDIANNESS_H

#include <util/typedefs.h>

namespace FunkyBoy::Util {

    inline u16 compose16Bits(u8 lsb, u8 msb) {
        return (msb << 8u) | lsb;
    }

    inline u16_fast compose16BitsFast(u8_fast lsb, u8_fast msb) {
        return (msb << 8u) | (lsb & 0xffu);
    }

}

#endif //FB_CORE_ENDIANNESS_H
