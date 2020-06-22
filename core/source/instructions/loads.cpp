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

#include "loads.h"

#include <util/endianness.h>
#include <util/registers.h>

using namespace FunkyBoy;

bool Instructions::load_r_r(InstrContext &context) {
    u8 &dst = context.registers[context.instr >> 3u & 7u];
    u8 src = context.registers[context.instr & 7u];
    dst = src;
    return true;
}

bool Instructions::load_mem_dd_A(InstrContext &context) {
    context.memory->write8BitsTo(Util::compose16Bits(context.lsb, context.msb), *context.regA);
    return true;
}

bool Instructions::load_A_mem_dd(InstrContext &context) {
    *context.regA = context.memory->read8BitsAt(Util::compose16Bits(context.lsb, context.msb));
    return true;
}

bool Instructions::load_A_d(InstrContext &context) {
    *context.regA = context.lsb;
    return true;
}

bool Instructions::load_dd_nn(InstrContext &context) {
    Util::write16BitRegister(context.registers, context.instr >> 4u & 3u, Util::compose16Bits(context.lsb, context.msb));
    return true;
}

bool Instructions::load_SP_nn(InstrContext &context) {
    context.stackPointer = Util::compose16Bits(context.lsb, context.msb);
    return true;
}

bool Instructions::load_nn_SP(InstrContext &context) {
    context.memory->write16BitsTo(Util::compose16Bits(context.lsb, context.msb), context.stackPointer);
    return true;
}

bool Instructions::load_r_n(InstrContext &context) {
    context.registers[context.instr >> 3 & 0b111] = context.lsb;
    return true;
}

bool Instructions::load_HL_n(InstrContext &context) {
    context.memory->write8BitsTo(context.readHL(), context.lsb);
    return true;
}

bool Instructions::load_reg_dd_A(InstrContext &context) {
    context.memory->write8BitsTo(Util::read16BitRegister(context.registers, context.instr >> 4 & 1), *context.regA);
    return true;
}

bool Instructions::load_A_reg_dd(InstrContext &context) {
    *context.regA = context.memory->read8BitsAt(Util::read16BitRegister(context.registers, context.instr >> 4 & 1));
    return true;
}

bool Instructions::load_HLI_A(InstrContext &context) {
    u16 hl = context.readHL();
    context.memory->write8BitsTo(hl, *context.regA);
    context.writeHL(hl + 1);
    return true;
}

bool Instructions::load_HLD_A(InstrContext &context) {
    u16 hl = context.readHL();
    context.memory->write8BitsTo(hl, *context.regA);
    context.writeHL(hl - 1);
    return true;
}

bool Instructions::load_A_HLI(InstrContext &context) {
    u16 hl = context.readHL();
    *context.regA = context.memory->read8BitsAt(hl);
    context.writeHL(hl + 1);
    return true;
}

bool Instructions::load_A_HLD(InstrContext &context) {
    u16 hl = context.readHL();
    *context.regA = context.memory->read8BitsAt(hl);
    context.writeHL(hl - 1);
    return true;
}

bool Instructions::load_HL_r(InstrContext &context) {
    // 0x70 -> 1110 000 -> B
    // 0x70 -> 1110 001 -> C
    // 0x70 -> 1110 010 -> D
    // 0x70 -> 1110 011 -> E
    // 0x70 -> 1110 100 -> H
    // 0x70 -> 1110 101 -> L
    // --- Skip F ---
    // 0x70 -> 1110 110 -> A
    u16 hl = context.readHL();
    context.memory->write8BitsTo(hl, context.registers[context.instr & 0b111u]);
    return true;
}

bool Instructions::load_r_HL(InstrContext &context) {
    // 0x46 -> 1 000 110 -> B
    // 0x4E -> 1 001 110 -> C
    // 0x56 -> 1 010 110 -> D
    // 0x5E -> 1 011 110 -> E
    // 0x66 -> 1 100 110 -> H
    // 0x6E -> 1 101 110 -> L
    // --- Skip F ---
    // 0x7E -> 1 111 110 -> A
    u16 hl = context.readHL();
    context.registers[(context.instr >> 3u) & 0b111u] = context.memory->read8BitsAt(hl);
    return true;
}

bool Instructions::load_SP_HL(InstrContext &context) {
    context.stackPointer = context.readHL();
    return true;
}

bool Instructions::load_HL_SPe(InstrContext &context) {
    context.writeHL(Util::addToSP(context.regF, context.stackPointer, context.signedByte));
    return true;
}