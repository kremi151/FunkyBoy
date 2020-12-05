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

#include <util/flags.h>
#include <util/registers.h>

namespace FunkyBoy::Instructions::ALU {

    inline void __alu_adc(u8_fast &flags, u8_fast &regA, u8_fast &val, bool carry) {
        u8_fast carryVal = carry ? 1 : 0;
        u8_fast newVal = regA + val + carryVal;
        Flags::setFlagsFast(flags, (newVal & 0xffu) == 0, false, ((regA & 0xf) + (val & 0xf) + carryVal) > 0xf,
                        (regA & 0xff) + (val & 0xff) + carryVal > 0xff);
        regA = newVal;
    }

    inline void __alu_addToHL(Instructions::context &context, u16_fast val) {
        u16_fast oldVal = context.readHL();
        u16_fast newVal = oldVal + val;

        Flags::setFlagsFast(*context.regF, Flags::isZeroFast(*context.regF), false,
                ((oldVal & 0xfff) + (val & 0xfff)) > 0xfff, (oldVal & 0xffff) + (val & 0xffff) > 0xffff);

        context.writeHL(newVal);
    }

    inline void __alu_sbc(u8_fast &flags, u8_fast &regA, u8_fast &val, bool carry) {
        u8_fast carryVal = carry ? 1 : 0;
        u8_fast newVal = regA - val - carryVal;
        Flags::setFlagsFast(flags, (newVal & 0xffu) == 0, true, (regA & 0xf) - (val & 0xf) - carryVal < 0, (regA & 0xffu) < ((val + carryVal) & 0xffu));
        regA = newVal;
    }

    inline void __alu_cp(u8_fast &flags, const u8_fast &regA, const u8_fast &val) {
        // See http://z80-heaven.wikidot.com/instructions-set:cp
        Flags::setFlagsFast(flags, (regA & 0xffu) == (val & 0xffu), true, (regA & 0xf) - (val & 0xf) < 0, (regA & 0xffu) < (val & 0xffu));
    }

    inline void __alu_or(u8_fast &flags, u8_fast &regA, u8_fast val) {
        regA |= val;
        Flags::setFlagsFast(flags, (regA & 0xffu) == 0, false, false, false);
    }

    inline void __alu_and(u8_fast &flags, u8_fast &regA, u8_fast val) {
        regA &= val;
        //TODO: To be verified:
        Flags::setFlagsFast(flags, (regA & 0xffu) == 0, false, true, false);
    }

    inline void __alu_xor(u8_fast &flags, u8_fast &regA, u8_fast val) {
        regA ^= val;
        Flags::setFlagsFast(flags, (regA & 0xffu) == 0, false, false, false);
    }

}

using namespace FunkyBoy::Instructions;

void ALU::add_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_adc(*context.regF, *context.regA, context.registers[opcode & 7u], false);
}

void ALU::adc_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_adc(*context.regF, *context.regA, context.registers[opcode & 7u], Flags::isCarryFast(*context.regF));
}

void ALU::add_A_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_adc(*context.regF, *context.regA, lsb, false);
}

void ALU::adc_A_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_adc(*context.regF, *context.regA, lsb, Flags::isCarryFast(*context.regF));
}

void ALU::add_HL_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    // 0x09 -> 00 1001 -> BC
    // 0x19 -> 01 1001 -> DE
    // 0x29 -> 10 1001 -> HL
    ALU::__alu_addToHL(context, context.read16BitRegister((opcode >> 4) & 0b11u));
}

void ALU::add_HL_SP(FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_addToHL(context, context.stackPointer);
}

void ALU::add_SP_e8(FunkyBoy::Memory &memory, Instructions::context &context) {
    i8_fast signedByte = memory.readSigned8BitsAt(context.progCounter++);
    context.stackPointer = Util::addToSPFast(*context.regF, context.stackPointer, signedByte);
}

void ALU::add_A_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast hl = memory.read8BitsAt(context.readHL());
    ALU::__alu_adc(*context.regF, *context.regA, hl, false);
}

void ALU::adc_A_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast hl = memory.read8BitsAt(context.readHL());
    ALU::__alu_adc(*context.regF, *context.regA, hl, Flags::isCarryFast(*context.regF));
}

void ALU::sub_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_sbc(*context.regF, *context.regA, context.registers[opcode & 7u], false);
}

void ALU::sbc_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_sbc(*context.regF, *context.regA, context.registers[opcode & 7u], Flags::isCarryFast(*context.regF));
}

void ALU::sub_A_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_sbc(*context.regF, *context.regA, lsb, false);
}

void ALU::sbc_A_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_sbc(*context.regF, *context.regA, lsb, Flags::isCarryFast(*context.regF));
}

void ALU::sub_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast hl = memory.read8BitsAt(context.readHL());
    ALU::__alu_sbc(*context.regF, *context.regA, hl, false);
}

void ALU::sbc_A_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast hl = memory.read8BitsAt(context.readHL());
    ALU::__alu_sbc(*context.regF, *context.regA, hl, Flags::isCarryFast(*context.regF));
}

void ALU::cp_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_cp(*context.regF, *context.regA, context.registers[opcode & 0b00000111u]);
}

void ALU::cp_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8 val = memory.read8BitsAt(context.readHL());
    ALU::__alu_cp(*context.regF, *context.regA, val);
}

void ALU::cp_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8 val = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_cp(*context.regF, *context.regA, val);
}

void ALU::inc_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast position = opcode >> 4u & 3u;
    u16_fast val = context.read16BitRegister(position);
    context.write16BitRegister(position, val + 1);
}

void ALU::inc_SP(Instructions::context &context) {
    context.stackPointer++;
}

void ALU::inc_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16_fast hl = context.readHL();
    u8_fast oldVal = memory.read8BitsAt(hl);
    u8_fast newVal = oldVal + 1;
    memory.write8BitsTo(hl, newVal);
    Flags::setZeroFast(*context.regF, (newVal & 0xffu) == 0);
    Flags::setHalfCarryFast(*context.regF, (newVal & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstractionFast(*context.regF, false);
    // Leave carry as-is
}

void ALU::inc_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    auto &reg = *(context.registers + (opcode >> 3u & 7u));
    reg++;
    Flags::setZeroFast(*context.regF, (reg & 0xffu) == 0);
    Flags::setHalfCarryFast(*context.regF, (reg & 0x0fu) == 0x00); // If half-overflow, 4 least significant bits will be 0
    Flags::setSubstractionFast(*context.regF, false);
    // Leave carry as-is
}

void ALU::dec_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast position = opcode >> 4u & 3u;
    u16_fast val = context.read16BitRegister(position);
    context.write16BitRegister(position, val - 1);
}

void ALU::dec_SP(Instructions::context &context) {
    context.stackPointer--;
}

void ALU::dec_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    u16_fast hl = context.readHL();
    u8_fast oldVal = memory.read8BitsAt(hl);
    u8_fast newVal = oldVal - 1;
    memory.write8BitsTo(hl, newVal);
    Flags::setZeroFast(*context.regF, (newVal & 0xffu) == 0);
    Flags::setHalfCarryFast(*context.regF, (newVal & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstractionFast(*context.regF, true);
    // Leave carry as-is
}

void ALU::dec_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    auto &reg = *(context.registers + (opcode >> 3 & 7));
    reg--;
    Flags::setZero(context.regF, reg == 0);
    Flags::setHalfCarry(context.regF, (reg & 0x0fu) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
    Flags::setSubstraction(context.regF, true);
    // Leave carry as-is
}

void ALU::or_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    // 0xB0 -> 10110 000 -> B
    // 0xB1 -> 10110 001 -> C
    // 0xB2 -> 10110 010 -> D
    // 0xB3 -> 10110 011 -> E
    // 0xB4 -> 10110 100 -> H
    // 0xB5 -> 10110 101 -> L
    // -- F is skipped --
    // 0xB7 -> 10110 111 -> A
    ALU::__alu_or(*context.regF, *context.regA, context.registers[opcode & 0b111u]);
}

void ALU::or_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_or(*context.regF, *context.regA, memory.read8BitsAt(context.readHL()));
}

void ALU::or_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_or(*context.regF, *context.regA, lsb);
}

void ALU::and_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    // 0xA0 -> 10100 000 -> B
    // 0xA1 -> 10100 001 -> C
    // 0xA2 -> 10100 010 -> D
    // 0xA3 -> 10100 011 -> E
    // 0xA4 -> 10100 100 -> H
    // 0xA5 -> 10100 101 -> L
    // -- F is skipped --
    // 0xA7 -> 10100 111 -> A
    ALU::__alu_and(*context.regF, *context.regA, context.registers[opcode & 0b111u]);
}

void ALU::and_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_and(*context.regF, *context.regA, memory.read8BitsAt(context.readHL()));
}

void ALU::and_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_and(*context.regF, *context.regA, lsb);
}

void ALU::xor_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_xor(*context.regF, *context.regA, context.registers[opcode & 0b111u]);
}

void ALU::xor_HL(FunkyBoy::Memory &memory, Instructions::context &context) {
    ALU::__alu_xor(*context.regF, *context.regA, memory.read8BitsAt(context.readHL()));
}

void ALU::xor_d8(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    ALU::__alu_xor(*context.regF, *context.regA, lsb);
}