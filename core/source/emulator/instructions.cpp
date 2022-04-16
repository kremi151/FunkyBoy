/**
 * Copyright 2022 Michel Kremer (kremi151)
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

#ifdef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include "instructions.h"
#include <operands/instruction_context.h>
#include <util/flags.h>
#include <util/registers.h>
#include <util/endianness.h>

#include <util/debug.h>

namespace FunkyBoy::Util {

    inline u16 read16BitsAt(Memory &memory, memory_address addr) {
        return compose16Bits(memory.read8BitsAt(addr), memory.read8BitsAt(addr + 1));
    }

}

// TODO: Maybe not the best idea...
using namespace FunkyBoy::Flags;

FunkyBoy::u8_fast FunkyBoy::InstrContext::doInstruction(Memory &memory) {
#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (instr & 0xff);
    file << " B=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regB & 0xff);
    file << " C=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regC & 0xff);
    file << " D=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regD & 0xff);
    file << " E=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regE & 0xff);
    file << " H=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regH & 0xff);
    file << " L=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regL & 0xff);
    file << " A=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regA & *regA);
    file << " F=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regF_do_not_use_directly & 0xff);
    file << " PC=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (progCounter - 1);
    file << " SP=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << stackPointer;
    file << std::endl;
    instr++;
#endif

    switch (instr) {
        // nop
        case 0x00: {
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            memory.write8BitsTo(read16BitRegister(instr >> 4 & 1), *regA);
            return true;
        }
            // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            *regA = memory.read8BitsAt(read16BitRegister(instr >> 4 & 1));
            return true;
        }
            // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            u16 hl = readHL();
            memory.write8BitsTo(hl, *regA);
            writeHL(hl + 1);
            return true;
        }
            // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            u16 hl = readHL();
            memory.write8BitsTo(hl, *regA);
            writeHL(hl - 1);
            return true;
        }
            // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            u16 hl = readHL();
            *regA = memory.read8BitsAt(hl);
            writeHL(hl + 1);
            return true;
        }
            // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            u16 hl = readHL();
            *regA = memory.read8BitsAt(hl);
            writeHL(hl - 1);
            return true;
        }
            // ld (HL),s
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            // 0x70 -> 1110 000 -> B
            // 0x70 -> 1110 001 -> C
            // 0x70 -> 1110 010 -> D
            // 0x70 -> 1110 011 -> E
            // 0x70 -> 1110 100 -> H
            // 0x70 -> 1110 101 -> L
            // --- Skip F ---
            // 0x70 -> 1110 110 -> A

            debug_print_4("ld (HL),s\n");
            memory.write8BitsTo(readHL(), registers[instr & 0b111]);
            return true;
        }
            // ld s,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            // 0x46 -> 1 000 110 -> B
            // 0x4E -> 1 001 110 -> C
            // 0x56 -> 1 010 110 -> D
            // 0x5E -> 1 011 110 -> E
            // 0x66 -> 1 100 110 -> H
            // 0x6E -> 1 101 110 -> L
            // --- Skip F ---
            // 0x7E -> 1 111 110 -> A
            registers[(instr >> 3) & 0b111] = memory.read8BitsAt(readHL());
            return true;
        }
            // ld SP,HL
        case 0xF9: {
            stackPointer = readHL();
            return true;
        }
            // ld HL,SP+e8
        case 0xF8: {
            auto signedByte = memory.readSigned8BitsAt(progCounter++);
            writeHL(Util::addToSP(regF, stackPointer, signedByte));
            return true;
        }
            // ldh (a8),A
        case 0xE0: {
            auto addr = memory.read8BitsAt(progCounter++);
            debug_print_4("ldh (a8),A 0x%04X <- 0x%02X\n", 0xFF00 + addr, *regA);
            memory.write8BitsTo(0xFF00 + addr, *regA);
            return true;
        }
            // ldh A,(a8)
        case 0xF0: {
            auto addr = memory.read8BitsAt(progCounter++);
            *regA = memory.read8BitsAt(0xFF00 + addr);
            debug_print_4("ldh A,(a8) A <- 0x%02X (0x%04X)\n", *regA & 0xff, 0xFF00 + addr);
            return true;
        }
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            debug_print_4("add reg,reg\n");
            bool carry = (instr & 8) && isCarry(regF);
            Instructions::adc(regF, regA, registers[instr & 7], carry);
            return true;
        }
            // add A,d8
        case 0xC6: {
            debug_print_4("add A,d8\n");
            u8 val = memory.read8BitsAt(progCounter++);
            Instructions::adc(regF, regA, val, false);
            return true;
        }
            // adc A,d8
        case 0xCE: {
            debug_print_4("adc A,d8\n");
            u8 val = memory.read8BitsAt(progCounter++);
            Instructions::adc(regF, regA, val, isCarry(regF));
            return true;
        }
            // add HL,ss
        case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            // 0x09 -> 00 1001 -> BC
            // 0x19 -> 01 1001 -> DE
            // 0x29 -> 10 1001 -> HL
            Instructions::addToHL(*this, read16BitRegister((instr >> 4) & 0b11));
            return true;
        }
            // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            Instructions::addToHL(*this, stackPointer);
            return true;
        }
            // add SP,r8
        case 0xE8: {
            auto signedByte = memory.readSigned8BitsAt(progCounter++);
            stackPointer = Util::addToSP(regF, stackPointer, signedByte);
            return true;
        }
            // add A,(HL)
        case 0x86: {
            Instructions::adc(regF, regA, memory.read8BitsAt(readHL()), false);
            return true;
        }
            // adc A,(HL)
        case 0x8E: {
            Instructions::adc(regF, regA, memory.read8BitsAt(readHL()), isCarry(regF));
            return true;
        }
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // sub a,reg
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: // sbc a,reg
        {
            debug_print_4("sub reg,reg\n");
            bool carry = (instr & 8) && isCarry(regF);
            Instructions::sbc(regF, regA, registers[instr & 7], carry);
            return true;
        }
            // sub A,d8
        case 0xD6: {
            debug_print_4("sub A,d8\n");
            u8 val = memory.read8BitsAt(progCounter++);
            Instructions::sbc(regF, regA, val, false);
            return true;
        }
            // sbc A,d8
        case 0xDE: {
            debug_print_4("sbc A,d8\n");
            u8 val = memory.read8BitsAt(progCounter++);
            Instructions::sbc(regF, regA, val, isCarry(regF));
            return true;
        }
            // sub (HL)
        case 0x96: {
            Instructions::sbc(regF, regA, memory.read8BitsAt(readHL()), false);
            return true;
        }
            // sbc (HL)
        case 0x9E: {
            Instructions::sbc(regF, regA, memory.read8BitsAt(readHL()), isCarry(regF));
            return true;
        }
            // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            bool set = instr & 0b00001000;
            u16 address = Util::read16BitsAt(memory, progCounter);
            progCounter += 2;
            if ((!set && !isZero(regF)) || (set && isZero(regF))) {
                debug_print_4("jp (N)Z a16 from 0x%04X", progCounter - 1);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
            // jp (N)C,a16
        case 0xD2: case 0xDA: {
            bool set = instr & 0b00001000;
            u16 address = Util::read16BitsAt(memory, progCounter);
            progCounter += 2;
            if ((!set && !isCarry(regF)) || (set && isCarry(regF))) {
                debug_print_4("jp (C)Z a16 from 0x%04X", progCounter - 1);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
            // unconditional jp
        case 0xC3:
        {
            debug_print_4("jp (unconditional) a16 from 0x%04X", progCounter);
            progCounter = Util::read16BitsAt(memory, progCounter);
            debug_print_4(" to 0x%04X\n", progCounter);
            return true;
        }
            // jp HL
        case 0xE9:
        {
            debug_print_4("jp HL\n");
            progCounter = readHL();
            return true;
        }
            // jr (N)Z,r8
        case 0x20: case 0x28: { // TODO: Can this branch bew combined with jp (N)Z,a16 ?
            bool set = instr & 0b00001000;
            auto signedByte = memory.readSigned8BitsAt(progCounter++);
            debug_print_4("jr (N)Z,r8 set ? %d %d\n", set, isZero(regF));
            if ((!set && !isZero(regF)) || (set && isZero(regF))) {
                debug_print_4("JR (N)Z from 0x%04X + %d", progCounter - 1, signedByte);
                progCounter += signedByte;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
            // jr (N)C,r8
        case 0x30: case 0x38: { // TODO: Can this branch bew combined with jp (N)C,a16 ?
            bool set = instr & 0b00001000;
            auto signedByte = memory.readSigned8BitsAt(progCounter++);
            debug_print_4("jr (N)C,r8 set ? %d %d\n", set, isCarry(regF));
            if ((!set && !isCarry(regF)) || (set && isCarry(regF))) {
                debug_print_4("JR (N)C from 0x%04X + %d", progCounter - 1, signedByte);
                progCounter += signedByte;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
            // unconditional jr
        case 0x18: {
            auto signedByte = memory.readSigned8BitsAt(progCounter++);
            debug_print_4("JR (unconditional) from 0x%04X + %d", progCounter, signedByte);
            progCounter += signedByte;
            debug_print_4(" to 0x%04X\n", progCounter);
            return true;
        }
            // call (N)Z,a16
        case 0xC4: case 0xCC: {
            bool set = instr & 0b00001000;
            debug_print_4("call (N)Z,a16 set ? %d %d\n", set, isZero(regF));
            u16 address = Util::read16BitsAt(memory, progCounter);
            progCounter += 2;
            if ((!set && !isZero(regF)) || (set && isZero(regF))) {
                debug_print_4("call from 0x%04X", progCounter - 2);
                push16Bits(memory, progCounter);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
            // call (N)C,a16
        case 0xD4: case 0xDC: {
            bool set = instr & 0b00001000;
            debug_print_4("call (N)C,a16 set ? %d %d\n", set, isCarry(regF));
            u16 address = Util::read16BitsAt(memory, progCounter);
            progCounter += 2;
            if ((!set && !isCarry(regF)) || (set && isCarry(regF))) {
                debug_print_4("call from 0x%04X", progCounter - 2);
                push16Bits(memory, progCounter);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        case 0xCD: {
            u16 address = Util::read16BitsAt(memory, progCounter);
            progCounter += 2;
            debug_print_4("call from 0x%04X\n", progCounter);
            push16Bits(memory, progCounter);
            progCounter = address;
            debug_print_4(" to 0x%04X\n", progCounter);
            return true;
        }
            // ret (N)Z,a16
        case 0xC0: case 0xC8: {
            bool set = instr & 0b00001000;
            debug_print_4("ret (N)Z,a16 set ? %d %d\n", set, isZero(regF));
            if ((!set && !isZero(regF)) || (set && isZero(regF))) {
                goto return_;
            }
            return true;
        }
            // ret (N)C,a16
        case 0xD0: case 0xD8: {
            bool set = instr & 0b00001000;
            debug_print_4("ret (N)C,a16 set ? %d %d\n", set, isCarry(regF));
            if ((!set && !isCarry(regF)) || (set && isCarry(regF))) {
                goto return_;
            }
            return true;
        }
            // ret a16
        case 0xC9: {
            debug_print_4("ret a16\n");
            return_:
            progCounter = pop16Bits(memory);
            return true;
        }
            // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            u8 rstAddr = (instr >> 3 & 7) * 8;
            debug_print_4("rst %02XH\n", rstAddr);
            push16Bits(memory, progCounter);
            progCounter = rstAddr;
            return true;
        }
            // cp s
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: {
            Instructions::cp(regF, regA, registers[instr & 0b00000111]);
            return true;
        }
            // cp A
        case 0xBF: {
            Instructions::cp(regF, regA, *regA);
            return true;
        }
            // cp HL
        case 0xBE: {
            Instructions::cp(regF, regA, memory.read8BitsAt(readHL()));
            return true;
        }
            // cp d8
        case 0xFE: {
            Instructions::cp(regF, regA, memory.read8BitsAt(progCounter++));
            return true;
        }
            // inc ss
        case 0x03: case 0x13: case 0x23: {
            u8 position = instr >> 4 & 3;
            u16 val = read16BitRegister(position);
            write16BitRegister(position, val + 1);
            return true;
        }
            // inc SP
        case 0x33: {
            stackPointer++;
            return true;
        }
            // inc (HL)
        case 0x34: {
            u16 hl = readHL();
            u8 oldVal = memory.read8BitsAt(hl);
            u8 newVal = oldVal + 1;
            memory.write8BitsTo(hl, newVal);
            setZero(regF, newVal == 0);
            setHalfCarry(regF, (newVal & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(regF, false);
            // Leave carry as-is
            return true;
        }
            // inc s
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: {
            auto reg = registers + (instr >> 3 & 7);
            (*reg)++;
            setZero(regF, *reg == 0);
            setHalfCarry(regF, (*reg & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(regF, false);
            // Leave carry as-is
            return true;
        }
            // inc A
        case 0x3C: {
            (*regA)++;
            setZero(regF, *regA == 0);
            setHalfCarry(regF, (*regA & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(regF, false);
            // Leave carry as-is
            return true;
        }
            // dec (HL)
        case 0x35: {
            u16 hl = readHL();
            u8 oldVal = memory.read8BitsAt(hl);
            u8 newVal = oldVal - 1;
            memory.write8BitsTo(hl, newVal);
            setZero(regF, newVal == 0);
            setHalfCarry(regF, (newVal & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(regF, true);
            // Leave carry as-is
            return true;
        }
            // dec s
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: {
            auto reg = registers + (instr >> 3 & 7);
            (*reg)--;
            setZero(regF, *reg == 0);
            setHalfCarry(regF, (*reg & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(regF, true);
            // Leave carry as-is
            return true;
        }
            // dec A
        case 0x3D: {
            (*regA)--;
            setZero(regF, *regA == 0);
            setHalfCarry(regF, (*regA & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(regF, true);
            // Leave carry as-is
            return true;
        }
            // dec ss
        case 0x0B: case 0x1B: case 0x2B: {
            u8 position = instr >> 4 & 3;
            u16 val = read16BitRegister(position);
            write16BitRegister(position, val - 1);
            return true;
        }
            // dec SP
        case 0x3B: {
            stackPointer--;
            return true;
        }
            // or s
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: {
            // 0xB0 -> 10110 000 -> B
            // 0xB1 -> 10110 001 -> C
            // 0xB2 -> 10110 010 -> D
            // 0xB3 -> 10110 011 -> E
            // 0xB4 -> 10110 100 -> H
            // 0xB5 -> 10110 101 -> L
            // -- F is skipped --
            // 0xB7 -> 10110 111 -> A
            Instructions::alu_or(regF, regA, registers[instr & 0b111]);
            return true;
        }
            // or (HL)
        case 0xB6: {
            Instructions::alu_or(regF, regA, memory.read8BitsAt(readHL()));
            return true;
        }
            // or d8
        case 0xF6: {
            Instructions::alu_or(regF, regA, memory.read8BitsAt(progCounter++));
            return true;
        }
            // and s
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: {
            // 0xA0 -> 10100 000 -> B
            // 0xA1 -> 10100 001 -> C
            // 0xA2 -> 10100 010 -> D
            // 0xA3 -> 10100 011 -> E
            // 0xA4 -> 10100 100 -> H
            // 0xA5 -> 10100 101 -> L
            // -- F is skipped --
            // 0xA7 -> 10100 111 -> A
            Instructions::alu_and(regF, regA, registers[instr & 0b111]);
            return true;
        }
            // and (HL)
        case 0xA6: {
            Instructions::alu_and(regF, regA, memory.read8BitsAt(readHL()));
            return true;
        }
            // and d8
        case 0xE6: {
            Instructions::alu_and(regF, regA, memory.read8BitsAt(progCounter++));
            return true;
        }
            // xor s
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            Instructions::alu_xor(regF, regA, registers[instr & 0b111]);
            return true;
        }
            // xor (HL)
        case 0xAE: {
            Instructions::alu_xor(regF, regA, memory.read8BitsAt(readHL()));
            return true;
        }
            // xor d8
        case 0xEE: {
            auto val = memory.read8BitsAt(progCounter++);
            Instructions::alu_xor(regF, regA, val);
            return true;
        }
            // rrca
        case 0x0F: {
            u8 a = *regA;
            *regA = (a >> 1) | ((a & 1) << 7);
            setFlags(regF, false, false, false, a & 1);
            return true;
        }
            // rlca
        case 0x07: {
            u8 a = *regA;
            *regA = (a << 1) | ((a & 128) >> 7);
            setFlags(regF, false, false, false, (a & 128) != 0);
            return true;
        }
            // rra
        case 0x1F: {
            u8 a = *regA;
            *regA = a >> 1;
            if (isCarry(regF)) {
                *regA |= 128; // (bit 7 set to 1)
            }
            setFlags(regF, false, false, false, a & 1);
            return true;
        }
            // rla
        case 0x17: {
            u8 a = *regA;
            *regA = a << 1;
            if (isCarry(regF)) {
                *regA |= 1; // (bit 0 set to 1)
            }
            setFlags(regF, false, false, false, (a & 128) != 0);
            return true;
        }
        case 0xC1: case 0xD1: case 0xE1: // pop ss
        case 0xC5: case 0xD5: case 0xE5: // push ss
        {
            // 0xC1 -> 11 00 00 01
            // 0xD1 -> 11 01 00 01
            // 0xE1 -> 11 10 00 01

            // 0xC5 -> 11 00 01 01
            // 0xD5 -> 11 01 01 01
            // 0xE5 -> 11 10 01 01

            u8 *reg = registers + (((instr >> 4) & 3) * 2); // 16-bit -> x 2
            bool push = instr & 4;
            // TODO: Verify -> reg = MSB ; reg+1 = LSB ?
            if (push) {
                push16Bits(memory, *reg, *(reg + 1));
            } else {
                u16 val = pop16Bits(memory);
                *reg = (val >> 8) & 0xff;
                *(reg + 1) = val & 0xff;
            }
            return true;
        }
        case 0xF1: // pop AF
        case 0xF5: // push AF
        {
            // 0xF1 -> 11110001
            // 0xF5 -> 11110101
            bool push = instr & 4;
            if (push) {
                push16Bits(memory, readAF());
            } else {
                u16_fast val = pop16Bits(memory);
                writeAF(val);
            }
            return true;
        }
            // reti
        case 0xD9: {
            interruptMasterEnable = IMEState::ENABLED; // Immediate enable without delay
            progCounter = pop16Bits(memory);
            return true;
        }
            // daa
        case 0x27: {
            u8 val = *regA;
            if (isSubstraction(regF)) {
                if (isCarry(regF)) {
                    val -= 0x60;
                }
                if (isHalfCarry(regF)) {
                    val -= 0x06;
                }
            } else {
                if (isCarry(regF) || val > 0x99) {
                    val += 0x60;
                    setCarry(regF, true);
                }
                if (isHalfCarry(regF) || (val & 0xf) > 0x09) {
                    val += 0x06;
                }
            }
            *regA = val;
            setZero(regF, val == 0);
            setHalfCarry(regF, false);
            return true;
        }
            // cpl
        case 0x2F: {
            *regA = ~*regA;
            setSubstraction(regF, true);
            setHalfCarry(regF, true);
            return true;
        }
            // scf
        case 0x37: {
            setFlags(regF, isZero(regF), false, false, true);
            return true;
        }
            // ccf
        case 0x3F: {
            setFlags(regF, isZero(regF), false, false, !isCarry(regF));
            return true;
        }
            // di
        case 0xF3: {
            interruptMasterEnable = IMEState::DISABLED;
            return true;
        }
            // ei
        case 0xFB: {
            interruptMasterEnable = IMEState::REQUEST_ENABLE;
            return true;
        }
            // stop
        case 0x10: {
            cpuState = CPUState::STOPPED;
            return true;
        }
            // halt
        case 0x76: {
            if (interruptMasterEnable == IMEState::ENABLED) {
                cpuState = CPUState::HALTED;
            } else if (gbType == GameBoyType::GameBoyCGB) {
                // On non-GBC devices, the next instruction is skipped if HALT was requested with IME being disabled
                progCounter++;
            }
            return true;
        }
        case 0xCB: {
            // This should already be handled in onTick, so we should never reach this case here
            return doPrefixInstruction(memory, memory.read8BitsAt(progCounter++));
        }
        default: {
            unknown_instr:
            fprintf(stderr,"Illegal instruction 0x%02X at 0x%04X\n", instr, progCounter - 1);
            return false;
        }
    }
}

FunkyBoy::u8_fast FunkyBoy::InstrContext::doPrefixInstruction(Memory &memory, u8_fast prefix) {
    switch(prefix) {
        // rlc reg
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg << 1) | ((*reg >> 7) & 0b1);
            setFlags(regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
            // rlc (HL)
        case 0x06: {
            u8 oldVal = memory.read8BitsAt(readHL());
            u8 newVal = (oldVal << 1) | ((oldVal >> 7) & 0b1);
            setFlags(regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // rrc reg
        case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg >> 1) | ((*reg & 0b1) << 7);
            setFlags(regF, newVal == 0, false, false, (*reg & 0b1) > 0);
            *reg = newVal;
            return true;
        }
            // rrc (HL)
        case 0x0E: {
            u8 oldVal = memory.read8BitsAt(readHL());
            u8 newVal = (oldVal >> 1) | ((oldVal & 0b1) << 7);
            setFlags(regF, newVal == 0, false, false, (oldVal & 0b1) > 0);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // rl reg
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg << 1);
            if (isCarry(regF)) {
                newVal |= 0b1;
            }
            setFlags(regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
            // rl (HL)
        case 0x16: {
            u8 oldVal = memory.read8BitsAt(readHL());
            u8 newVal = (oldVal << 1);
            if (isCarry(regF)) {
                newVal |= 0b1;
            }
            setFlags(regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // rr reg
        case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1F: {
            // 0x18 -> 11 000 -> B
            // 0x19 -> 11 001 -> C
            // 0x1A -> 11 010 -> D
            // 0x1B -> 11 011 -> E
            // 0x1C -> 11 100 -> H
            // 0x1D -> 11 101 -> L
            // --- Skip F ---
            // 0x1F -> 11 111 -> A
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = *reg >> 1;
            if (isCarry(regF)) {
                newVal |= 0b10000000;
            }
            setFlags(regF, newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
            // rr (HL)
        case 0x1E: {
            u8 oldVal = Util::read16BitsAt(memory, readHL());
            u8 newVal = oldVal >> 1;
            if (isCarry(regF)) {
                newVal |= 0b10000000;
            }
            setFlags(regF, newVal == 0, false, false, oldVal & 0b1);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // sla reg
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27: {
            // 0x20 -> 100 000 -> B
            // 0x21 -> 100 001 -> C
            // 0x22 -> 100 010 -> D
            // 0x23 -> 100 011 -> E
            // 0x24 -> 100 100 -> H
            // 0x25 -> 100 101 -> L
            // --- Skip F ---
            // 0x27 -> 100 111 -> A
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = *reg << 1;
            setFlags(regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
            // sla (HL)
        case 0x26: {
            u8 oldVal = Util::read16BitsAt(memory, readHL());
            u8 newVal = oldVal << 1;
            setFlags(regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // sra reg
        case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg >> 1) | (*reg & 0b10000000);
            setFlags(regF, newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
            // sra (HL)
        case 0x2E: {
            u8 oldVal = memory.read8BitsAt(readHL());
            u8 newVal = (oldVal >> 1) | (oldVal & 0b10000000);
            setFlags(regF, newVal == 0, false, false, oldVal & 0b1);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
            // swap reg
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: {
            u8 *reg = registers + (prefix & 0b111);
            *reg = ((*reg >> 4) & 0b1111) | ((*reg & 0b1111) << 4);
            setFlags(regF, *reg == 0, false, false, false);
            return true;
        }
            // swap (HL)
        case 0x36: {
            u8 val = memory.read8BitsAt(readHL());
            val = ((val >> 4) & 0b1111) | ((val & 0b1111) << 4);
            memory.write8BitsTo(readHL(), val);
            setFlags(regF, val == 0, false, false, false);
            return true;
        }
            // srl reg
        case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3F: {
            // 0x38 -> 111 000 -> B
            // 0x39 -> 111 001 -> C
            // 0x3A -> 111 010 -> D
            // 0x3B -> 111 011 -> E
            // 0x3C -> 111 100 -> H
            // 0x3D -> 111 101 -> L
            // --- Skip F ---
            // 0x3F -> 111 111 -> A
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = *reg >> 1;
            setFlags(regF, newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
            // srl (HL)
        case 0x3E: {
            u8 oldVal = Util::read16BitsAt(memory, readHL());
            u8 newVal = oldVal >> 1;
            setFlags(regF, newVal == 0, false, false, oldVal & 0b1);
            memory.write8BitsTo(readHL(), newVal);
            return true;
        }
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // bit 0,reg
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F: // bit 1,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // bit 2,reg
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F: // bit 3,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // bit 4,reg
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: // bit 5,reg
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: // bit 6,reg
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: // bit 7,reg
        {
            // 0x40 -> 1 000 000 -> 0,B
            // 0x41 -> 1 000 001 -> 0,C
            // 0x42 -> 1 000 010 -> 0,D
            // 0x43 -> 1 000 011 -> 0,E
            // 0x44 -> 1 000 100 -> 0,H
            // 0x45 -> 1 000 101 -> 0,L
            // --- Skip F ---
            // 0x47 -> 1 000 111 -> 0,A

            // 0x48 -> 1 001 000 -> 1,B
            // 0x49 -> 1 001 001 -> 1,C
            // 0x4A -> 1 001 010 -> 1,D
            // 0x4B -> 1 001 011 -> 1,E
            // 0x4C -> 1 001 100 -> 1,H
            // 0x4D -> 1 001 101 -> 1,L
            // --- Skip F ---
            // 0x4F -> 1 001 111 -> 1,A

            // 0x50 -> 1 010 000 -> 2,B
            // 0x51 -> 1 010 001 -> 2,C
            // 0x52 -> 1 010 010 -> 2,D
            // 0x53 -> 1 010 011 -> 2,E
            // 0x54 -> 1 010 100 -> 2,H
            // 0x55 -> 1 010 101 -> 2,L
            // --- Skip F ---
            // 0x57 -> 1 010 111 -> 2,A

            // etc...

            u8 bitShift = (prefix >> 3) & 0b111;
            u8 bitMask = 1 << bitShift;
            u8 regPos = prefix & 0b111;
            u8 *reg = registers + regPos;
            // Note: We write the opposite of the Nth bit into the Z flag
            setFlags(regF, !(*reg & bitMask), false, true, isCarry(regF));
            return true;
        }
            // bit n,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x76: case 0x7E: {
            // 0x46 -> 1 000 110 -> 0
            // 0x4E -> 1 001 110 -> 1
            // 0x56 -> 1 010 110 -> 2
            // 0x5E -> 1 011 110 -> 3
            // 0x66 -> 1 100 110 -> 4
            // 0x6E -> 1 101 110 -> 5
            // 0x76 -> 1 110 110 -> 6
            // 0x7E -> 1 111 110 -> 6
            u8 bitShift = (prefix >> 3) & 0b111;
            u8 bitMask = 1 << bitShift;
            // Note: We write the opposite of the Nth bit into the Z flag
            setFlags(regF, !(memory.read8BitsAt(readHL()) & bitMask), false, true, isCarry(regF));
            return true;
        }
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // res 0,reg
        case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F: // res 1,reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // res 2,reg
        case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // res 3,reg
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: // res 4,reg
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: // res 5,reg
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: // res 6,reg
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: // res 7,reg
        {
            u8 bitShift = (prefix >> 3) & 0b111;
            u8 regPos = prefix & 0b111;
            u8 *reg = registers + regPos;
            *reg &= ~(1 << bitShift);
            return true;
        }
            // res n,(HL)
        case 0x86: case 0x8E: case 0x96: case 0x9E: case 0xA6: case 0xAE: case 0xB6: case 0xBE: {
            u8 bitShift = (prefix >> 3) & 0b111;
            u8 val = memory.read8BitsAt(readHL());
            val &= ~(1 << bitShift);
            memory.write8BitsTo(readHL(), val);
            return true;
        }
        case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7: // set 0,reg
        case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF: // set 1,reg
        case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7: // set 2,reg
        case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF: // set 3,reg
        case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7: // set 4,reg
        case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF: // set 5,reg
        case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7: // set 6,reg
        case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF: // set 7,reg
        {
            u8 bitMask = (prefix >> 3) & 0b111;
            u8 regPos = prefix & 0b111;
            u8 *reg = registers + regPos;
            *reg |= (1 << bitMask);
            return true;
        }
            // set n,(HL)
        case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE: {
            u8 bitMask = (prefix >> 3) & 0b111;
            u8 val = memory.read8BitsAt(readHL());
            val |= (1 << bitMask);
            memory.write8BitsTo(readHL(), val);
            return true;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", prefix, progCounter - 1);
            return false;
        }
    }
}

#endif
