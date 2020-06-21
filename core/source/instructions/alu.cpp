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

inline void __alu_addToHL(u8 *flags, u8 *regH, u8 *regL, u16 val) {
    u16 oldVal = Util::composeHL(*regH, *regL);
    u16 newVal = oldVal + val;

    Flags::setFlags(flags, Flags::isZero(flags), false, ((oldVal & 0xfffu) + (val & 0xfffu)) > 0xfff, (oldVal & 0xffffu) + (val & 0xffffu) > 0xffff);

    Util::writeHL(*regH, *regL, newVal);
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
    __alu_addToHL(context.regF, context.regH, context.regL, Util::read16BitRegister(context.registers, (context.instr >> 4) & 0b11u));
}

void Instructions::add_HL_SP(InstrContext &context) {
    __alu_addToHL(context.regF, context.regH, context.regL, context.stackPointer);
}

void Instructions::add_SP_e(InstrContext &context) {
    context.stackPointer = Util::addToSP(context.regF, context.stackPointer, context.signedByte);
}

void Instructions::add_A_HL(InstrContext &context) {
    u16 hl = Util::composeHL(*context.regH, *context.regL);
    __alu_adc(context.regF, context.regA, context.memory->read8BitsAt(hl), false);
}

void Instructions::adc_A_HL(InstrContext &context) {
    u16 hl = Util::composeHL(*context.regH, *context.regL);
    __alu_adc(context.regF, context.regA, context.memory->read8BitsAt(hl), Flags::isCarry(context.regF));
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
    u16 hl = Util::composeHL(*context.regH, *context.regL);
    __alu_sbc(context.regF, context.regA, context.memory->read8BitsAt(hl), false);
}

void Instructions::sbc_A_HL(InstrContext &context) {
    u16 hl = Util::composeHL(*context.regH, *context.regL);
    __alu_sbc(context.regF, context.regA, context.memory->read8BitsAt(hl), Flags::isCarry(context.regF));
}