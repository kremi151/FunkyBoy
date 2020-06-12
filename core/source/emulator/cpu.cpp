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

#include "cpu.h"

#include <iostream>
#include <utility>
#include <util/typedefs.h>

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
#include <iomanip>
#endif

using namespace FunkyBoy;

CPU::CPU(std::shared_ptr<Memory> memory): progCounter(0), stackPointer(0xFFFE), memory(std::move(memory))
#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    , file("exec_opcodes_fb_v2.txt")
#endif
{
    regB = registers;
    regC = registers + 1;
    regD = registers + 2;
    regE = registers + 3;
    regH = registers + 4;
    regL = registers + 5;
    regF_do_not_use_directly = registers + 6;
    regA = registers + 7;

    // Initialize registers

    // AF -> 0x01b0
    if (isGbc()) {
        *regA = 0x11;
    } else {
        *regA = 0x01;
    }
    *regF_do_not_use_directly = 0xb0;

    // BC -> 0x0013
    *regB = 0x00;
    *regC = 0x13;

    // DE -> 0x00d8
    *regD = 0x00;
    *regE = 0xd8;

    // HL -> 0x014d
    *regH = 0x01;
    *regL = 0x4d;
}

inline bool CPU::isCarry() {
    return *regF_do_not_use_directly & 0b00010000;
}

void CPU::setCarry(bool carry) {
    if (carry) {
        *regF_do_not_use_directly |= 0b00010000;
    } else {
        *regF_do_not_use_directly &= 0b11100000;
    }
}

inline bool CPU::isHalfCarry() {
    return *regF_do_not_use_directly & 0b00100000;
}

void CPU::setHalfCarry(bool halfCarry) {
    if (halfCarry) {
        *regF_do_not_use_directly |= 0b00100000;
    } else {
        *regF_do_not_use_directly &= 0b11010000;
    }
}

inline bool CPU::isSubstraction() {
    return *regF_do_not_use_directly & 0b01000000;
}

void CPU::setSubstraction(bool substration) {
    if (substration) {
        *regF_do_not_use_directly |= 0b01000000;
    } else {
        *regF_do_not_use_directly &= 0b10110000;
    }
}

inline bool CPU::isZero() {
    return *regF_do_not_use_directly & 0b10000000;
}

void CPU::setZero(bool zero) {
    if (zero) {
        *regF_do_not_use_directly |= 0b10000000;
    } else {
        *regF_do_not_use_directly &= 0b01110000;
    }
}

bool CPU::isGbc() {
    // TODO: Implement
    return false;
}

void CPU::setFlags(bool zero, bool subtraction, bool halfCarry, bool carry) {
    if (zero) {
        *regF_do_not_use_directly |= 0b10000000;
    } else {
        *regF_do_not_use_directly &= 0b01110000;
    }
    if (subtraction) {
        *regF_do_not_use_directly |= 0b01000000;
    } else {
        *regF_do_not_use_directly &= 0b10110000;
    }
    if (halfCarry) {
        *regF_do_not_use_directly |= 0b00100000;
    } else {
        *regF_do_not_use_directly &= 0b11010000;
    }
    if (carry) {
        *regF_do_not_use_directly |= 0b00010000;
    } else {
        *regF_do_not_use_directly &= 0b11100000;
    }
}

bool CPU::doTick() {
    auto currOffset = progCounter;
    auto opcode = memory->read8BitsAt(progCounter++);

    debug_print_4("> instr 0x%02X at 0x%04X\n", opcode, currOffset);

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (opcode & 0xff);
    file << " B=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regB & 0xff);
    file << " C=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regC & 0xff);
    file << " D=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regD & 0xff);
    file << " E=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regE & 0xff);
    file << " H=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regH & 0xff);
    file << " L=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regL & 0xff);
    file << " A=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regA & *regA);
    file << " F=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*regF_do_not_use_directly & 0xff);
    file << std::endl;
#endif

    switch (opcode) {
        // nop
        case 0x00:
            debug_print_4("nop\n");
            return true;
        // ld reg,reg
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            debug_print_4("ld reg,reg\n");
            u8 &dst = registers[opcode >> 3 & 7];
            u8 src = registers[opcode & 7];
            dst = src;
            return true;
        }
        // ld (a16),A
        case 0xEA: {
            auto destAddr = memory->read16BitsAt(progCounter);
            progCounter += 2;
            memory->write8BitsTo(destAddr, *regA);
            return true;
        }
        // ld A,(a16)
        case 0xFA: {
            auto srcAddr = memory->read16BitsAt(progCounter);
            progCounter += 2;
            *regA = memory->read8BitsAt(srcAddr);
            return true;
        }
        // ld (C),A
        case 0xE2: {
            memory->write8BitsTo(0xFF00 + *regC, *regA);
            return true;
        }
        // ld A,(C)
        case 0xF2: {
            *regA = memory->read8BitsAt(0xFF00 + *regC);
            return true;
        }
        // ld A,d8
        case 0x3E: {
            *regA = memory->read8BitsAt(progCounter++);
            debug_print_4("ldh A,d8 A <- 0x%02X\n", *regA);
            return true;
        }
        // ld (ss),d16
        case 0x01: case 0x11: case 0x21: {
            debug_print_4("ld (ss),d16\n");
            write16BitRegister(opcode >> 4 & 3, memory->read16BitsAt(progCounter));
            progCounter += 2;
            return true;
        }
        // ld SP,d16
        case 0x31: {
            debug_print_4("ld SP,d16\n");
            stackPointer = memory->read16BitsAt(progCounter); // TODO: Correct?
            progCounter += 2;
            return true;
        }
        // ld (a16),SP
        case 0x08: {
            debug_print_4("ld (a16),SP\n");
            memory->write16BitsTo(memory->read16BitsAt(progCounter), stackPointer);
            progCounter += 2;
            return true;
        }
        // ld s,d8
        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print_4("ld s,d8\n");
            *(registers + (opcode >> 3 & 5)) = memory->read8BitsAt(progCounter++);
            return true;
        }
        // ld (HL),d8
        case 0x36: {
            debug_print_4("ld (HL),d8\n");
            memory->write8BitsTo(readHL(), memory->read8BitsAt(progCounter++)); // TODO: Correct?
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            memory->write8BitsTo(read16BitRegister(opcode >> 4 & 1), *regA);
            return true;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            *regA = memory->read8BitsAt(read16BitRegister(opcode >> 4 & 1));
            return true;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            u16 hl = readHL();
            memory->write8BitsTo(hl, *regA);
            writeHL(hl + 1);
            return true;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            u16 hl = readHL();
            memory->write8BitsTo(hl, *regA);
            writeHL(hl - 1);
            return true;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            u16 hl = readHL();
            *regA = memory->read8BitsAt(hl);
            writeHL(hl + 1);
            return true;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            u16 hl = readHL();
            *regA = memory->read8BitsAt(hl);
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
            memory->write8BitsTo(readHL(), registers[opcode & 0b111]);
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
            registers[(opcode >> 3) & 0b111] = memory->read8BitsAt(readHL());
            return true;
        }
        // ldh (a8),A
        case 0xE0: {
            auto addr = memory->read8BitsAt(progCounter++);
            debug_print_4("ldh (a8),A 0x%04X <- 0x%02X\n", 0xFF00 + addr, *regA);
            memory->write8BitsTo(0xFF00 + addr, *regA);
            return true;
        }
        // ldh A,(a8)
        case 0xF0: {
            auto addr = memory->read8BitsAt(progCounter++);
            *regA = memory->read8BitsAt(0xFF00 + addr);
            debug_print_4("ldh A,(a8) A <- 0x%02X (0x%04X)\n", *regA & 0xff, 0xFF00 + addr);
            return true;
        }
        // add reg,reg TODO: Correct notation?
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            debug_print_4("add reg,reg\n");
            bool carry = (opcode & 8) && isCarry();
            adc(registers[opcode & 7], carry);
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            bool set = opcode & 0b00001000;
            u16 address = memory->read16BitsAt(progCounter++);
            if ((!set && !isZero()) || (set && isZero())) {
                debug_print_4("jp (N)Z a16 from 0x%04X", progCounter - 1);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            bool set = opcode & 0b00001000;
            u16 address = memory->read16BitsAt(progCounter++);
            if ((!set && !isCarry()) || (set && isCarry())) {
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
            progCounter = memory->read16BitsAt(progCounter);
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
            bool set = opcode & 0b00001000;
            auto signedByte = memory->readSigned8BitsAt(progCounter++);
            debug_print_4("jr (N)Z,r8 set ? %d %d\n", set, isZero());
            if ((!set && !isZero()) || (set && isZero())) {
                debug_print_4("JR (N)Z from 0x%04X + %d", progCounter - 1, signedByte);
                progCounter += signedByte;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: { // TODO: Can this branch bew combined with jp (N)C,a16 ?
            bool set = opcode & 0b00001000;
            auto signedByte = memory->readSigned8BitsAt(progCounter++);
            debug_print_4("jr (N)C,r8 set ? %d %d\n", set, isCarry());
            if ((!set && !isCarry()) || (set && isCarry())) {
                debug_print_4("JR (N)C from 0x%04X + %d", progCounter - 1, signedByte);
                progCounter += signedByte;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        // unconditional jr
        case 0x18: {
            auto signedByte = memory->readSigned8BitsAt(progCounter++);
            debug_print_4("JR (unconditional) from 0x%04X + %d", progCounter, signedByte);
            progCounter += signedByte;
            debug_print_4(" to 0x%04X\n", progCounter);
            return true;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            bool set = opcode & 0b00001000;
            debug_print_4("call (N)Z,a16 set ? %d %d\n", set, isZero());
            u16 address = memory->read16BitsAt(progCounter++);
            if ((!set && !isZero()) || (set && isZero())) {
                debug_print_4("call from 0x%04X", progCounter - 1);
                push16Bits(progCounter + 2);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            bool set = opcode & 0b00001000;
            debug_print_4("call (N)C,a16 set ? %d %d\n", set, isCarry());
            u16 address = memory->read16BitsAt(progCounter++);
            if ((!set && !isCarry()) || (set && isCarry())) {
                debug_print_4("call from 0x%04X", progCounter - 1);
                push16Bits(progCounter + 2);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        case 0xCD: {
            debug_print_4("call from 0x%04X", progCounter);
            u16 address = memory->read16BitsAt(progCounter);
            push16Bits(progCounter + 2);
            progCounter = address;
            debug_print_4(" to 0x%04X\n", progCounter);
            return true;
        }
        // ret (N)Z,a16
        case 0xC0: case 0xC8: {
            bool set = opcode & 0b00001000;
            debug_print_4("ret (N)Z,a16 set ? %d %d\n", set, isZero());
            if ((!set && !isZero()) || (set && isZero())) {
                goto return_;
            }
            return true;
        }
        // ret (N)C,a16
        case 0xD0: case 0xD8: {
            bool set = opcode & 0b00001000;
            debug_print_4("ret (N)C,a16 set ? %d %d\n", set, isCarry());
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto return_;
            }
            return true;
        }
        // ret a16
        case 0xC9: {
            debug_print_4("ret a16\n");
return_:
            progCounter = pop16Bits();
            return true;
        }
        // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            u8 rstAddr = (opcode >> 3 & 7) * 8;
            debug_print_4("rst %02XH\n", rstAddr);
            push16Bits(progCounter);
            progCounter = rstAddr;
            return true;
        }
        // cp s
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: {
            cp(registers[opcode & 0b00000111]);
            return true;
        }
        // cp A
        case 0xBF: {
            cp(*regA);
            return true;
        }
        // cp HL
        case 0xBE: {
            cp(memory->read8BitsAt(readHL()));
            return true;
        }
        // cp d8
        case 0xFE: {
            cp(memory->read8BitsAt(progCounter++));
            return true;
        }
        // inc ss
        case 0x03: case 0x13: case 0x23: {
            u8 position = opcode >> 4 & 3;
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
            u8 oldVal = memory->read8BitsAt(hl);
            u8 newVal = oldVal + 1;
            memory->write8BitsTo(hl, newVal);
            setZero(newVal == 0);
            setHalfCarry((newVal & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // inc s
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: {
            auto reg = registers + (opcode >> 3 & 7);
            (*reg)++;
            setZero(*reg == 0);
            setHalfCarry((*reg & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // inc A
        case 0x3C: {
            (*regA)++;
            setZero(*regA == 0);
            setHalfCarry((*regA & 0x0f) == 0x00); // If half-overflow, 4 least significant bits will be 0
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // dec (HL)
        case 0x35: {
            u16 hl = readHL();
            u8 oldVal = memory->read8BitsAt(hl);
            u8 newVal = oldVal - 1;
            memory->write8BitsTo(hl, newVal);
            setZero(newVal == 0);
            setHalfCarry((newVal & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(true);
            // Leave carry as-is
            return true;
        }
        // dec s
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: {
            auto reg = registers + (opcode >> 3 & 7);
            (*reg)--;
            setZero(*reg == 0);
            setHalfCarry((*reg & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(true);
            // Leave carry as-is
            return true;
        }
        // dec A
        case 0x3D: {
            (*regA)--;
            setZero(*regA == 0);
            setHalfCarry((*regA & 0x0f) == 0x0f); // If half-underflow, 4 least significant bits will turn from 0000 (0x0) to 1111 (0xf)
            setSubstraction(true);
            // Leave carry as-is
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
            _or(registers[opcode & 0b111]);
            return true;
        }
        // or (HL)
        case 0xB6: {
            _or(memory->read8BitsAt(readHL()));
            return true;
        }
        // or d8
        case 0xF6: {
            _or(memory->read8BitsAt(progCounter++));
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
            _and(registers[opcode & 0b111]);
            return true;
        }
        // and (HL)
        case 0xA6: {
            _and(memory->read8BitsAt(readHL()));
            return true;
        }
        // and d8
        case 0xE6: {
            _and(memory->read8BitsAt(progCounter++));
            return true;
        }
        // xor s
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            _xor(registers[opcode & 0b111]);
            return true;
        }
        // xor (HL)
        case 0xAE: {
            _xor(memory->read8BitsAt(readHL()));
            return true;
        }
        // xor d8
        case 0xEE: {
            auto val = memory->read8BitsAt(progCounter++);
            _xor(val);
            return true;
        }
        // rrca
        case 0x0F: {
            u8 a = *regA;
            *regA = (a >> 1) | ((a & 1) << 7);
            // Explanation for Z flag: if regA is already 0, it will remain zero. Otherwise, it will be reset.
            setFlags(a == 0, false, false, a & 1);
            return true;
        }
        // rlca
        case 0x07: {
            u8 a = *regA;
            *regA = (a << 1) | ((a & 128) >> 7);
            // Explanation for Z flag: if regA is already 0, it will remain zero. Otherwise, it will be reset.
            setFlags(a == 0, false, false, (a & 128) != 0);
            return true;
        }
        // rra
        case 0x1F: {
            u8 a = *regA;
            *regA = a >> 1;
            if (isCarry()) {
                *regA |= 128; // (bit 7 set to 1)
            }
            setFlags(*regA == 0, false, false, a & 1);
            return true;
        }
        // rla
        case 0x17: {
            u8 a = *regA;
            *regA = a << 1;
            if (isCarry()) {
                *regA |= 1; // (bit 0 set to 1)
            }
            setFlags(*regA == 0, false, false, (a & 128) != 0);
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

            u8 *reg = registers + (((opcode >> 4) & 3) * 2); // 16-bit -> x 2
            bool push = opcode & 4;
            // TODO: Verify -> reg = MSB ; reg+1 = LSB ?
            if (push) {
                push16Bits(*reg, *(reg + 1));
            } else {
                u16 val = pop16Bits();
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
            bool push = opcode & 4;
            if (push) {
                push16Bits(readAF());
            } else {
                u16 val = pop16Bits();
                writeAF(val);
            }
            return true;
        }
        // reti
        case 0xD9: {
            progCounter = pop16Bits();
            memory->write8BitsTo(FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER, 1);
            return true;
        }
        // di
        case 0xF3: {
            memory->write8BitsTo(FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER, 0);
            return true;
        }
        // ei
        case 0xFB: {
            memory->write8BitsTo(FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER, 1);
            return true;
        }
        case 0xCB: {
            return doPrefix(opcode);
        }
        default: {
            unknown_instr:
            fprintf(stderr,"Illegal instruction 0x%02X at 0x%04X\n", opcode, progCounter - 1);
            return false;
        }
    }
}

bool CPU::doPrefix(u8 prefix) {
    fprintf(stderr, "Encountered not yet implemented prefix 0x%02X followed by instruction 0x%02X\n", prefix, memory->read8BitsAt(progCounter++));
    return false;
}

void CPU::setProgramCounter(u16 offset) {
    progCounter = offset;
}

void CPU::push16Bits(FunkyBoy::u16 val) {
    stackPointer -= 2;
    memory->write8BitsTo(stackPointer, (u8) (val));
    memory->write8BitsTo(stackPointer + 1, (u8) (val >> 8u));
}

void CPU::push16Bits(FunkyBoy::u8 msb, FunkyBoy::u8 lsb) {
    stackPointer -= 2;
    memory->write8BitsTo(stackPointer, lsb);
    memory->write8BitsTo(stackPointer + 1, msb);
}

u16 CPU::pop16Bits() {
    u16 val = memory->read8BitsAt(stackPointer) | (memory->read8BitsAt(stackPointer + 1) << 8u);
    stackPointer += 2;
    return val;
}

u16 CPU::readHL() {
    return (*regL & 0xff) | (*regH << 8);
}

void CPU::writeHL(FunkyBoy::u16 val) {
    *regL = val & 0xff;
    *regH = (val >> 8) & 0xff;
}

u16 CPU::readAF() {
    return (*regF_do_not_use_directly & 0b11110000) | (*regA << 8);
}

void CPU::writeAF(FunkyBoy::u16 val) {
    *regF_do_not_use_directly = val & 0b11110000; // Only the 4 most significant bits are written to register F
    *regA = (val >> 8) & 0xff;
}

u16 CPU::read16BitRegister(FunkyBoy::u8 position) {
    u8 *reg = registers + (position * 2);
    return (*reg << 8) | (*(reg + 1) & 0xff);
}

void CPU::write16BitRegister(FunkyBoy::u8 position, FunkyBoy::u16 val) {
    u8 *reg = registers + (position * 2);
    *reg = (val >> 8) & 0xff;
    *(reg + 1) = val & 0xff;
}

void CPU::cp(u8 val) {
    debug_print_4("cp 0x%02X - 0x%02X\n", *regA, val);
    // See http://z80-heaven.wikidot.com/instructions-set:cp
    setFlags(*regA == val, true, (*regA & 0xF) - (val & 0xF) < 0, *regA < val);
}

void CPU::_xor(u8 val) {
    debug_print_4("xor 0x%02X ^ 0x%02X\n", *regA, val);
    *regA ^= val;
    setFlags(*regA == 0, false, false, false);
}

void CPU::_or(FunkyBoy::u8 val) {
    debug_print_4("or 0x%02X | 0x%02X\n", *regA, val);
    *regA |= val;
    setFlags(*regA == 0, false, false, false);
}

void CPU::_and(FunkyBoy::u8 val) {
    debug_print_4("and 0x%02X & 0x%02X\n", *regA, val);
    *regA &= val;
    //TODO: To be verified:
    setFlags(*regA == 0, false, true, false);
}

inline void CPU::adc(u8 val, bool carry) {
    if (carry) val++;
    u8 newVal = *regA + val;
    setFlags(newVal == 0, false, ((*regA & 0xf) + (val & 0xf)) > 0xf, (*regA & 0xff) + (val & 0xff) > 0xff);
    *regA = newVal;
}