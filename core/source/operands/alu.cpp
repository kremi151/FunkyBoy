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

#include "alu.h"

#include <util/typedefs.h>
#include <util/registers.h>
#include <util/flags.h>

using namespace FunkyBoy;

inline void __alu_adc(u8 *flags, u8 *regA, u8 val, bool carry) {
    u8 carryVal = carry ? 1 : 0;
    u8 newVal = *regA + val + carryVal;
    Flags::setFlags(flags, newVal == 0, false, ((*regA & 0xf) + (val & 0xf) + carryVal) > 0xf, (*regA & 0xff) + (val & 0xff) + carryVal > 0xff);
    *regA = newVal;
}

inline void __alu_sbc(u8 *flags, u8 *regA, u8 val, bool carry) {
    u8 carryVal = carry ? 1 : 0;
    u8 newVal = *regA - val - carryVal;
    Flags::setFlags(flags, newVal == 0, true, (*regA & 0xf) - (val & 0xf) - carryVal < 0, *regA < (val + carryVal));
    *regA = newVal;
}

inline void __alu_addToHL(InstrContext &context, u16 val) {
    u16 oldVal = context.readHL();
    u16 newVal = oldVal + val;

    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, ((oldVal & 0xfff) + (val & 0xfff)) > 0xfff, (oldVal & 0xffff) + (val & 0xffff) > 0xffff);

    context.writeHL(newVal);
}

inline void __alu_cp(u8 *flags, const u8 *regA, u8 val) {
    // See http://z80-heaven.wikidot.com/instructions-set:cp
    Flags::setFlags(flags, *regA == val, true, (*regA & 0xf) - (val & 0xf) < 0, *regA < val);
}

inline void __alu_or(u8 *flags, u8 *regA, u8 val) {
    *regA |= val;
    Flags::setFlags(flags, *regA == 0, false, false, false);
}

inline void __alu_and(u8 *flags, u8 *regA, u8 val) {
    *regA &= val;
    //TODO: To be verified:
    Flags::setFlags(flags, *regA == 0, false, true, false);
}

inline void __alu_xor(u8 *flags, u8 *regA, u8 val) {
    *regA ^= val;
    Flags::setFlags(flags, *regA == 0, false, false, false);
}

bool Operands::add_A_r(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, context.registers[context.instr & 7u], false);
    return true;
}

bool Operands::add_A_d(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, context.lsb, false);
    return true;
}

bool Operands::adc_A_r(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, context.registers[context.instr & 7u], Flags::isCarry(context.regF));
    return true;
}

bool Operands::adc_A_d(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, context.lsb, Flags::isCarry(context.regF));
    return true;
}

bool Operands::add_HL_ss(InstrContext &context, Memory &memory) {
    // 0x09 -> 00 1001 -> BC
    // 0x19 -> 01 1001 -> DE
    // 0x29 -> 10 1001 -> HL
    __alu_addToHL(context, context.read16BitRegister((context.instr >> 4) & 0b11u));
    return true;
}

bool Operands::add_HL_SP(InstrContext &context, Memory &memory) {
    __alu_addToHL(context, context.stackPointer);
    return true;
}

bool Operands::add_SP_e(InstrContext &context, Memory &memory) {
    context.stackPointer = Util::addToSP(context.regF, context.stackPointer, context.signedByte);
    return true;
}

bool Operands::add_A_HL(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, memory.read8BitsAt(context.readHL()), false);
    return true;
}

bool Operands::adc_A_HL(InstrContext &context, Memory &memory) {
    __alu_adc(context.regF, context.regA, memory.read8BitsAt(context.readHL()), Flags::isCarry(context.regF));
    return true;
}

bool Operands::sub_A_r(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, context.registers[context.instr & 7u], false);
    return true;
}

bool Operands::sub_A_d(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, context.lsb, false);
    return true;
}

bool Operands::sbc_A_r(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, context.registers[context.instr & 7u], Flags::isCarry(context.regF));
    return true;
}

bool Operands::sbc_A_d(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, context.lsb, Flags::isCarry(context.regF));
    return true;
}

bool Operands::sub_HL(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, memory.read8BitsAt(context.readHL()), false);
    return true;
}

bool Operands::sbc_A_HL(InstrContext &context, Memory &memory) {
    __alu_sbc(context.regF, context.regA, memory.read8BitsAt(context.readHL()), Flags::isCarry(context.regF));
    return true;
}

bool Operands::cp_r(InstrContext &context, Memory &memory) {
    __alu_cp(context.regF, context.regA, context.registers[context.instr & 0b00000111u]);
    return true;
}

bool Operands::cp_d(InstrContext &context, Memory &memory) {
    __alu_cp(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::cp_HL(InstrContext &context, Memory &memory) {
    u8 val = memory.read8BitsAt(context.readHL());
    __alu_cp(context.regF, context.regA, val);
    return true;
}

bool Operands::inc_ss(InstrContext &context, Memory &memory) {
    u8 position = context.instr >> 4u & 3u;
    u16 val = context.read16BitRegister(position);
    context.write16BitRegister(position, val + 1);
    return true;
}

bool Operands::inc_SP(InstrContext &context, Memory &memory) {
    context.stackPointer++;
    return true;
}

bool Operands::inc_HL(InstrContext &context, Memory &memory) {
    u16 hl = context.readHL();
    u8 oldVal = memory.read8BitsAt(hl);
    u8 newVal = oldVal + 1;
    memory.write8BitsTo(hl, newVal);
    Flags::setZero(context.regF, newVal == 0);
    Flags::setHalfCarry(context.regF, (newVal & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstraction(context.regF, false);
    // Leave carry as-is
    return true;
}

bool Operands::inc_r(InstrContext &context, Memory &memory) {
    auto reg = context.registers + (context.instr >> 3u & 7u);
    (*reg)++;
    Flags::setZero(context.regF, *reg == 0);
    Flags::setHalfCarry(context.regF, (*reg & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstraction(context.regF, false);
    // Leave carry as-is
    return true;
}

bool Operands::dec_ss(InstrContext &context, Memory &memory) {
    u8 position = context.instr >> 4u & 3u;
    u16 val = context.read16BitRegister(position);
    context.write16BitRegister(position, val - 1);
    return true;
}

bool Operands::dec_SP(InstrContext &context, Memory &memory) {
    context.stackPointer--;
    return true;
}

bool Operands::dec_HL(InstrContext &context, Memory &memory) {
    u16 hl = context.readHL();
    u8 oldVal = memory.read8BitsAt(hl);
    u8 newVal = oldVal - 1;
    memory.write8BitsTo(hl, newVal);
    Flags::setZero(context.regF, newVal == 0);
    Flags::setHalfCarry(context.regF, (newVal & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstraction(context.regF, true);
    // Leave carry as-is
    return true;
}

bool Operands::dec_r(InstrContext &context, Memory &memory) {
    auto reg = context.registers + (context.instr >> 3 & 7);
    (*reg)--;
    Flags::setZero(context.regF, *reg == 0);
    Flags::setHalfCarry(context.regF, (*reg & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstraction(context.regF, true);
    // Leave carry as-is
    return true;
}

bool Operands::or_r(InstrContext &context, Memory &memory) {
    // 0xB0 -> 10110 000 -> B
    // 0xB1 -> 10110 001 -> C
    // 0xB2 -> 10110 010 -> D
    // 0xB3 -> 10110 011 -> E
    // 0xB4 -> 10110 100 -> H
    // 0xB5 -> 10110 101 -> L
    // -- F is skipped --
    // 0xB7 -> 10110 111 -> A
    __alu_or(context.regF, context.regA, context.registers[context.instr & 0b111u]);
    return true;
}

bool Operands::or_d(InstrContext &context, Memory &memory) {
    __alu_or(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::or_HL(InstrContext &context, Memory &memory) {
    __alu_or(context.regF, context.regA, memory.read8BitsAt(context.readHL()));
    return true;
}

bool Operands::and_r(InstrContext &context, Memory &memory) {
    // 0xA0 -> 10100 000 -> B
    // 0xA1 -> 10100 001 -> C
    // 0xA2 -> 10100 010 -> D
    // 0xA3 -> 10100 011 -> E
    // 0xA4 -> 10100 100 -> H
    // 0xA5 -> 10100 101 -> L
    // -- F is skipped --
    // 0xA7 -> 10100 111 -> A
    __alu_and(context.regF, context.regA, context.registers[context.instr & 0b111u]);
    return true;
}

bool Operands::and_d(InstrContext &context, Memory &memory) {
    __alu_and(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::and_HL(InstrContext &context, Memory &memory) {
    __alu_and(context.regF, context.regA, memory.read8BitsAt(context.readHL()));
    return true;
}

bool Operands::xor_r(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, context.registers[context.instr & 0b111u]);
    return true;
}

bool Operands::xor_d(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::xor_HL(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, memory.read8BitsAt(context.readHL()));
    return true;
}

#endif
