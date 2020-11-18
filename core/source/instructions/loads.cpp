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

void Loads::ld_SP_d16(FunkyBoy::Memory &memory, context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    context.stackPointer = Util::compose16Bits(lsb, msb);
}

void Loads::ld_a16_SP(FunkyBoy::Memory &memory, context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    memory_address address = Util::compose16Bits(lsb, msb);
    memory.write8BitsTo(address, context.stackPointer & 0xffu);
    memory.write8BitsTo(address + 1, (context.stackPointer >> 8) & 0xffu);
}

void Loads::ld_r_d8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    context.registers[(opcode >> 3) & 0b111u] = memory.read8BitsAt(context.progCounter++);
}

void Loads::ld_HL_d8(FunkyBoy::Memory &memory, context &context) {
    memory.write8BitsTo(context.readHL() & 0xffffu, memory.read8BitsAt(context.progCounter++));
}

void Loads::ld_ss_A(opcode_t opcode, FunkyBoy::Memory &memory, context &context) {
    memory.write8BitsTo(context.read16BitRegister((opcode >> 4) & 1), *context.regA);
}

void Loads::ld_A_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    *context.regA = memory.read8BitsAt(context.read16BitRegister((opcode >> 4) & 1));
}

void Loads::ld_HLI_A(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16 hl = context.readHL();
    memory.write8BitsTo(hl, *context.regA);
    context.writeHL(hl + 1);
}

void Loads::ld_HLD_A(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16 hl = context.readHL();
    memory.write8BitsTo(hl, *context.regA);
    context.writeHL(hl - 1);
}

void Loads::ld_A_HLI(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16 hl = context.readHL();
    *context.regA = memory.read8BitsAt(hl);
    context.writeHL(hl + 1);
}

void Loads::ld_A_HLD(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16 hl = context.readHL();
    *context.regA = memory.read8BitsAt(hl);
    context.writeHL(hl - 1);
}