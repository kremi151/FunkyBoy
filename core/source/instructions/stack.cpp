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

#include <util/registers.h>
#include "stack.h"

using namespace FunkyBoy::Instructions;

void Stack::pop_rr(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast *reg = Util::decodeRRAddressFromOpcodeFast(context.registers, opcode);
    *(reg + 1) = memory.read8BitsAt(context.stackPointer++); // LSB
    *reg = memory.read8BitsAt(context.stackPointer++); // MSB
}

void Stack::pop_AF(FunkyBoy::Memory &memory, Instructions::context &context) {
    *context.regF = memory.read8BitsAt(context.stackPointer++) & 0b11110000u; // LSB ; Only store 4 most significant bits into register F
    *context.regA = memory.read8BitsAt(context.stackPointer++); // MSB
}

void Stack::push_rr(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast *reg = Util::decodeRRAddressFromOpcodeFast(context.registers, opcode);
    memory.write8BitsTo(context.stackPointer - 1, *reg);
    memory.write8BitsTo(context.stackPointer - 2, *(reg + 1));
    context.stackPointer -= 2;
}

void Stack::push_AF(FunkyBoy::Memory &memory, Instructions::context &context) {
    memory.write8BitsTo(context.stackPointer - 1, *context.regA);
    memory.write8BitsTo(context.stackPointer - 2, *context.regF);
    context.stackPointer -= 2;
}