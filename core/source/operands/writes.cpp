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

#ifndef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include "writes.h"

#include <util/registers.h>

using namespace FunkyBoy;

bool Operands::write16BitsIntoRR(InstrContext &context, Memory &memory) {
    u8 *reg = Util::decodeRRAddressFromOpcode(context.registers, context.instr);
    *reg = context.msb;
    *(reg + 1) = context.lsb;
    return true;
}

bool Operands::write16BitsIntoAF(InstrContext &context, Memory &memory) {
    *context.regA = context.msb;
    *context.regF = context.lsb & 0b11110000u; // Only store 4 most significant bits into register F
    return true;
}

bool Operands::writeLSBIntoHLMem(InstrContext &context, Memory &memory) {
    memory.write8BitsTo(context.readHL(), context.lsb);
    return true;
}

#endif
