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

    [[deprecated]]
    void setFlags(u8 *flags, bool zero, bool subtraction, bool halfCarry, bool carry);

    void setFlagsFast(u8_fast &flags, bool zero, bool subtraction, bool halfCarry, bool carry);

    [[deprecated]]
    inline bool isCarry(const u8 *flags) {
        return *flags & 0b00010000u;
    }

    inline bool isCarryFast(const u8_fast &flags) {
        return flags & 0b00010000u;
    }

    [[deprecated]]
    void setCarry(u8 *flags, bool carry);

    [[deprecated]]
    inline bool isHalfCarry(const u8 *flags) {
        return *flags & 0b00100000u;
    }

    inline bool isHalfCarryFast(const u8_fast &flags) {
        return flags & 0b00100000u;
    }

    [[deprecated]]
    void setHalfCarry(u8 *flags, bool halfCarry);

    void setHalfCarryFast(u8_fast &flags, bool halfCarry);

    [[deprecated]]
    inline bool isSubstraction(const u8 *flags) {
        return *flags & 0b01000000u;
    }

    inline bool isSubstractionFast(const u8_fast &flags) {
        return flags & 0b01000000u;
    }

    [[deprecated]]
    void setSubstraction(u8 *flags, bool substration);

    void setSubstractionFast(u8_fast &flags, bool substration);

    [[deprecated]]
    inline bool isZero(const u8 *flags) {
        return *flags & 0b10000000u;
    }

    inline bool isZeroFast(const u8_fast &flags) {
        return flags & 0b10000000u;
    }

    [[deprecated]]
    void setZero(u8 *flags, bool zero);

    void setZeroFast(u8_fast &flags, bool zero);

}

#endif //FB_CORE_UTIL_FLAGS_H
