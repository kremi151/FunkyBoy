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

using namespace FunkyBoy::Instructions;

void RotShifts::rrca(Instructions::context &context) {
    u8_fast a = *context.regA & 0xffu;
    *context.regA = (a >> 1) | ((a & 1) << 7);
    Flags::setFlagsFast(*context.regF, false, false, false, a & 1u);
}

void RotShifts::rlca(Instructions::context &context) {
    u8_fast a = *context.regA & 0xffu;
    *context.regA = (a << 1) | ((a & 128) >> 7);
    Flags::setFlagsFast(*context.regF, false, false, false, (a & 128u) != 0);
}

void RotShifts::rra(Instructions::context &context) {
    u8_fast a = *context.regA & 0xffu;
    *context.regA = a >> 1;
    if (Flags::isCarryFast(*context.regF)) {
        *context.regA |= 128u; // (bit 7 set to 1)
    }
    Flags::setFlagsFast(*context.regF, false, false, false, a & 1u);
}

void RotShifts::rla(Instructions::context &context) {
    u8_fast a = *context.regA;
    *context.regA = (a << 1) & 0xffu;
    if (Flags::isCarryFast(*context.regF)) {
        *context.regA |= 1u; // (bit 0 set to 1)
    }
    Flags::setFlagsFast(*context.regF, false, false, false, (a & 128u) != 0);
}