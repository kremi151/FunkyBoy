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

#include <util/flags.h>
#include "misc.h"

using namespace FunkyBoy::Instructions;

void Miscellaneous::daa(Instructions::context &context) {
    u8_fast val = *context.regA & 0xffu;
    if (Flags::isSubstractionFast(*context.regF)) {
        if (Flags::isCarryFast(*context.regF)) {
            val -= 0x60;
        }
        if (Flags::isHalfCarryFast(*context.regF)) {
            val -= 0x06;
        }
    } else {
        if (Flags::isCarryFast(*context.regF) || val > 0x99) {
            val += 0x60;
            Flags::setHalfCarryFast(*context.regF, true);
        }
        if (Flags::isHalfCarryFast(*context.regF) || (val & 0xf) > 0x09) {
            val += 0x06;
        }
    }
    *context.regA = val;
    Flags::setZeroFast(*context.regF, (val & 0xffu) == 0);
    Flags::setHalfCarryFast(*context.regF, false);
}

void Miscellaneous::cpl(Instructions::context &context) {
    *context.regA = ~*context.regA;
    Flags::setSubstractionFast(*context.regF, true);
    Flags::setHalfCarryFast(*context.regF, true);
}

void Miscellaneous::scf(Instructions::context &context) {
    Flags::setFlagsFast(*context.regF, Flags::isZeroFast(*context.regF), false, false, true);
}

void Miscellaneous::ccf(Instructions::context &context) {
    Flags::setFlagsFast(*context.regF, Flags::isZeroFast(*context.regF), false, false, !Flags::isCarryFast(*context.regF));
}