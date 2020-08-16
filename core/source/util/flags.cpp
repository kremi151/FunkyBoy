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

#include "flags.h"

using namespace FunkyBoy;

void Flags::setFlags(u8 *flags, bool zero, bool subtraction, bool halfCarry, bool carry) {
    if (zero) {
        *flags |= 0b10000000u;
    } else {
        *flags &= 0b01110000u;
    }
    if (subtraction) {
        *flags |= 0b01000000u;
    } else {
        *flags &= 0b10110000u;
    }
    if (halfCarry) {
        *flags |= 0b00100000u;
    } else {
        *flags &= 0b11010000u;
    }
    if (carry) {
        *flags |= 0b00010000u;
    } else {
        *flags &= 0b11100000u;
    }
}

void Flags::setCarry(u8 *flags, bool carry) {
    if (carry) {
        *flags |= 0b00010000u;
    } else {
        *flags &= 0b11100000u;
    }
}

void Flags::setHalfCarry(u8 *flags, bool halfCarry) {
    if (halfCarry) {
        *flags |= 0b00100000u;
    } else {
        *flags &= 0b11010000u;
    }
}

void Flags::setSubstraction(u8 *flags, bool substration) {
    if (substration) {
        *flags |= 0b01000000u;
    } else {
        *flags &= 0b10110000u;
    }
}

void Flags::setZero(u8 *flags, bool zero) {
    if (zero) {
        *flags |= 0b10000000u;
    } else {
        *flags &= 0b01110000u;
    }
}