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

#include "rot_shifts.h"

#include <util/flags.h>

using namespace FunkyBoy;

bool Instructions::rrca(InstrContext &context) {
    u8 a = *context.regA;
    *context.regA = (a >> 1) | ((a & 1) << 7);
    Flags::setFlags(context.regF, false, false, false, a & 1u);
    return true;
}

bool Instructions::rlca(InstrContext &context) {
    u8 a = *context.regA;
    *context.regA = (a << 1) | ((a & 128) >> 7);
    Flags::setFlags(context.regF, false, false, false, (a & 128u) != 0);
    return true;
}

bool Instructions::rra(InstrContext &context) {
    u8 a = *context.regA;
    *context.regA = a >> 1;
    if (Flags::isCarry(context.regF)) {
        *context.regA |= 128u; // (bit 7 set to 1)
    }
    Flags::setFlags(context.regF, false, false, false, a & 1u);
    return true;
}

bool Instructions::rla(InstrContext &context) {
    u8 a = *context.regA;
    *context.regA = a << 1;
    if (Flags::isCarry(context.regF)) {
        *context.regA |= 1u; // (bit 0 set to 1)
    }
    Flags::setFlags(context.regF, false, false, false, (a & 128u) != 0);
    return true;
}