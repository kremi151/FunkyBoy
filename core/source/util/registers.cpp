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

#include "registers.h"

#include <util/flags.h>

using namespace FunkyBoy;

inline void Util::write16BitRegister(u8 *registers, u8 position, u16 val) {
    u8 *reg = registers + (position * 2);
    *reg = (val >> 8u) & 0xffu;
    *(reg + 1) = val & 0xffu;
}

inline u16 Util::read16BitRegister(u8 *registers, u8 position) {
    u8 *reg = registers + (position * 2);
    return (*reg << 8u) | (*(reg + 1u) & 0xffu);
}

inline u16 Util::composeHL(u8 &h, u8 &l) {
    return (l & 0xffu) | (h << 8u);
}

void Util::writeHL(u8 &h, u8 &l, u16 val) {
    l = val & 0xffu;
    h = (val >> 8u) & 0xffu;
}

u16 Util::addToSP(u8 *flags, u16 stackPointer, i8 val) {
    u16 newVal = stackPointer + val;

    // Note: Z flag is explicitly reset
    Flags::setFlags(flags, false, false, ((stackPointer & 0xfu) + (val & 0xfu)) > 0xfu, (stackPointer & 0xffu) + (val & 0xffu) > 0xffu);

    return newVal;
}