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

#include <memory/memory.h>
#include "loads.h"

using namespace FunkyBoy::Instructions;

void Loads::ld_reg_reg(opcode_t opcode, context &context) {
    u8_fast &dst = context.registers[(opcode >> 3u) & 7u];
    dst = context.registers[opcode & 7u];
}

void Loads::ld_a16_A(FunkyBoy::Memory &memory, context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    memory.write8BitsTo(Util::compose16Bits(lsb, msb), *context.regA);
}

void Loads::ld_A_a16(FunkyBoy::Memory &memory, context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    *context.regA = memory.read8BitsAt(Util::compose16Bits(lsb, msb));
}

void Loads::ld_C_A(FunkyBoy::Memory &memory, context &context) {
    memory.write8BitsTo(Util::compose16Bits(*context.regC, 0xFF), *context.regA);
}

void Loads::ld_A_C(FunkyBoy::Memory &memory, context &context) {
    *context.regA = memory.read8BitsAt(Util::compose16Bits(*context.regC, 0xFF));
}

void Loads::ld_A_d8(FunkyBoy::Memory &memory, context &context) {
    *context.regA = memory.read8BitsAt(context.progCounter++);
}

void Loads::ld_ss_d16(opcode_t opcode, FunkyBoy::Memory &memory, context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    context.write16BitRegister((opcode >> 4u) & 3u, Util::compose16Bits(lsb, msb));
}