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

#include "conditions.h"

#include <util/flags.h>

using namespace FunkyBoy;

bool Operands::checkIsZeroContextual(InstrContext &context, Memory &memory) {
    if (((context.instr & 0x0f) < 0x08)) {
        return !Flags::isZero(context.regF);
    } else {
        return Flags::isZero(context.regF);
    }
}

bool Operands::checkIsCarryContextual(InstrContext &context, Memory &memory) {
    if (((context.instr & 0x0f) < 0x08)) {
        return !Flags::isCarry(context.regF);
    } else {
        return Flags::isCarry(context.regF);
    }
}
