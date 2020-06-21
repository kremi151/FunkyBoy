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

#ifndef FB_CORE_UTIL_FLAGS_H
#define FB_CORE_UTIL_FLAGS_H

#include <util/typedefs.h>

namespace FunkyBoy::Flags {

    void setFlags(u8 *flags, bool zero, bool subtraction, bool halfCarry, bool carry);

    bool isCarry(const u8 *flags);
    void setCarry(u8 *flags, bool carry);

    bool isHalfCarry(const u8 *flags);
    void setHalfCarry(u8 *flags, bool halfCarry);

    bool isSubstraction(const u8 *flags);
    void setSubstraction(u8 *flags, bool substration);

    bool isZero(const u8 *flags);
    void setZero(u8 *flags, bool zero);

}

#endif //FB_CORE_UTIL_FLAGS_H
