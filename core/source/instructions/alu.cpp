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

#include "alu.h"

#include <util/typedefs.h>
#include <util/registers.h>
#include <util/flags.h>

using namespace FunkyBoy;

inline void __alu_adc(u8 *flags, u8 *regA, u8 val, bool carry) {
    u8 carryVal = carry ? 1 : 0;
    u8 newVal = *regA + val + carryVal;
    Flags::setFlags(flags, newVal == 0, false, ((*regA & 0xfu) + (val & 0xfu) + carryVal) > 0xf, (*regA & 0xffu) + (val & 0xffu) + carryVal > 0xff);
    *regA = newVal;
}

inline void __alu_sbc(u8 *flags, u8 *regA, u8 val, bool carry) {
    u8 carryVal = carry ? 1 : 0;
    u8 newVal = *regA - val - carryVal;
    Flags::setFlags(flags, newVal == 0, true, (*regA & 0xfu) - (val & 0xfu) - carryVal < 0, *regA < (val + carryVal));
    *regA = newVal;
}

inline void __alu_addToHL(InstrContext &context, u16 val) {
    u16 oldVal = context.readHL();
    u16 newVal = oldVal + val;

    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, ((oldVal & 0xfffu) + (val & 0xfffu)) > 0xfff, (oldVal & 0xffffu) + (val & 0xffffu) > 0xffff);

    context.writeHL(newVal);
}

inline void __alu_cp(u8 *flags, const u8 *regA, u8 val) {
    // See http://z80-heaven.wikidot.com/instructions-set:cp
    Flags::setFlags(flags, *regA == val, true, (*regA & 0xfu) - (val & 0xfu) < 0, *regA < val);
}

void Instructions::add_A_r(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.registers[context.instr & 7u], false);
}

void Instructions::add_A_d(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.lsb, false);
}

void Instructions::adc_A_r(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.registers[context.instr & 7u], Flags::isCarry(context.regF));
}

void Instructions::adc_A_d(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.lsb, Flags::isCarry(context.regF));
}

void Instructions::add_HL_ss(InstrContext &context) {
    // 0x09 -> 00 1001 -> BC
    // 0x19 -> 01 1001 -> DE
    // 0x29 -> 10 1001 -> HL
    __alu_addToHL(context, Util::read16BitRegister(context.registers, (context.instr >> 4) & 0b11u));
}

void Instructions::add_HL_SP(InstrContext &context) {
    __alu_addToHL(context, context.stackPointer);
}

void Instructions::add_SP_e(InstrContext &context) {
    context.stackPointer = Util::addToSP(context.regF, context.stackPointer, context.signedByte);
}

void Instructions::add_A_HL(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.memory->read8BitsAt(context.readHL()), false);
}

void Instructions::adc_A_HL(InstrContext &context) {
    __alu_adc(context.regF, context.regA, context.memory->read8BitsAt(context.readHL()), Flags::isCarry(context.regF));
}

void Instructions::sub_A_r(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.registers[context.instr & 7u], false);
}

void Instructions::sub_A_d(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.lsb, false);
}

void Instructions::sbc_A_r(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.registers[context.instr & 7u], Flags::isCarry(context.regF));
}

void Instructions::sbc_A_d(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.lsb, Flags::isCarry(context.regF));
}

void Instructions::sub_HL(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.memory->read8BitsAt(context.readHL()), false);
}

void Instructions::sbc_A_HL(InstrContext &context) {
    __alu_sbc(context.regF, context.regA, context.memory->read8BitsAt(context.readHL()), Flags::isCarry(context.regF));
}

void Instructions::cp_r(InstrContext &context) {
    __alu_cp(context.regF, context.regA, context.registers[context.instr & 0b00000111u]);
}

void Instructions::cp_d(InstrContext &context) {
    __alu_cp(context.regF, context.regA, context.lsb);
}

void Instructions::cp_HL(InstrContext &context) {
    u8 val = context.memory->read8BitsAt(context.readHL());
    __alu_cp(context.regF, context.regA, val);
}

void Instructions::inc_ss(InstrContext &context) {
    u8 position = context.instr >> 4u & 3u;
    u16 val = Util::read16BitRegister(context.registers, position);
    Util::write16BitRegister(context.registers, position, val + 1);
}

void Instructions::inc_SP(InstrContext &context) {
    context.stackPointer++;
}

void Instructions::inc_HL(InstrContext &context) {
    u16 hl = context.readHL();
    u8 oldVal = context.memory->read8BitsAt(hl);
    u8 newVal = oldVal + 1;
    context.memory->write8BitsTo(hl, newVal);
    Flags::setZero(context.regF, newVal == 0);
    Flags::setHalfCarry(context.regF, (newVal & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstraction(context.regF, false);
    // Leave carry as-is
}

void Instructions::inc_r(InstrContext &context) {
    auto reg = context.registers + (context.instr >> 3u & 7u);
    (*reg)++;
    Flags::setZero(context.regF, *reg == 0);
    Flags::setHalfCarry(context.regF, (*reg & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstraction(context.regF, false);
    // Leave carry as-is
}

void Instructions::dec_ss(InstrContext &context) {
    u8 position = context.instr >> 4u & 3u;
    u16 val = Util::read16BitRegister(context.registers, position);
    Util::write16BitRegister(context.registers, position, val - 1);
}

void Instructions::dec_SP(InstrContext &context) {
    context.stackPointer--;
}

void Instructions::dec_HL(InstrContext &context) {
    u16 hl = context.readHL();
    u8 oldVal = context.memory->read8BitsAt(hl);
    u8 newVal = oldVal - 1;
    context.memory->write8BitsTo(hl, newVal);
    Flags::setZero(context.regF, newVal == 0);
    Flags::setHalfCarry(context.regF, (newVal & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstraction(context.regF, true);
    // Leave carry as-is
}

void Instructions::dec_r(InstrContext &context) {
    auto reg = context.registers + (context.instr >> 3 & 7);
    (*reg)--;
    Flags::setZero(context.regF, *reg == 0);
    Flags::setHalfCarry(context.regF, (*reg & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstraction(context.regF, true);
    // Leave carry as-is
}