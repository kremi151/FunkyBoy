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
    , file("exec_opcodes_fb_v2.txt"), instr(0)
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
    powerUpInit();
}

void CPU::powerUpInit() {
    // Ref: https://gbdev.io/pandocs/#power-up-sequence

    // AF -> 0x01b0
    if (getType() == GameBoyType::GameBoyCGB) {
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

    stackPointer = 0xFFFE;

    memory->write8BitsTo(0xff05, 0x00);
    memory->write8BitsTo(0xff06, 0x00);
    memory->write8BitsTo(0xff07, 0x00);
    memory->write8BitsTo(0xff10, 0x80);
    memory->write8BitsTo(0xff11, 0xbf);
    memory->write8BitsTo(0xff12, 0xf3);
    memory->write8BitsTo(0xff14, 0xbf);
    memory->write8BitsTo(0xff16, 0x3f);
    memory->write8BitsTo(0xff17, 0x00);
    memory->write8BitsTo(0xff19, 0xbf);
    memory->write8BitsTo(0xff1a, 0x7f);
    memory->write8BitsTo(0xff1b, 0xff);
    memory->write8BitsTo(0xff1c, 0x9f);
    memory->write8BitsTo(0xff1e, 0xbf);
    memory->write8BitsTo(0xff20, 0xff);
    memory->write8BitsTo(0xff21, 0x00);
    memory->write8BitsTo(0xff22, 0x00);
    memory->write8BitsTo(0xff23, 0xbf);
    memory->write8BitsTo(0xff24, 0x77);
    memory->write8BitsTo(0xff25, 0xf3);
    if (getType() == GameBoyDMG) {
        memory->write8BitsTo(0xff26, 0xf1);
    } else {
        // TODO: This is for SGB, does this also apply for CGB?
        memory->write8BitsTo(0xff26, 0xf0);
    }
    memory->write8BitsTo(0xff40, 0x91);
    memory->write8BitsTo(0xff42, 0x00);
    memory->write8BitsTo(0xff43, 0x00);
    memory->write8BitsTo(0xff45, 0x00);
    memory->write8BitsTo(0xff47, 0xfc);
    memory->write8BitsTo(0xff48, 0xff);
    memory->write8BitsTo(0xff49, 0xff);
    memory->write8BitsTo(0xff4a, 0x00);
    memory->write8BitsTo(0xff4b, 0x00);
    memory->write8BitsTo(0xffff, 0x00);
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

GameBoyType CPU::getType() {
    // TODO: Implement
    return GameBoyType::GameBoyDMG;
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
    file << " PC=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (progCounter - 1);
    file << " SP=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << stackPointer;
    file << std::endl;
    instr++;
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
        // ld SP,HL
        case 0xF9: {
            stackPointer = readHL();
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
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            debug_print_4("add reg,reg\n");
            bool carry = (opcode & 8) && isCarry();
            adc(registers[opcode & 7], carry);
            return true;
        }
        // add A,d8
        case 0xC6: {
            debug_print_4("add A,d8\n");
            u8 val = memory->read8BitsAt(progCounter++);
            adc(val, false);
            return true;
        }
        // adc A,d8
        case 0xCE: {
            debug_print_4("adc A,d8\n");
            u8 val = memory->read8BitsAt(progCounter++);
            adc(val, isCarry());
            return true;
        }
        // add HL,ss
        case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            // 0x09 -> 00 1001 -> BC
            // 0x19 -> 01 1001 -> DE
            // 0x29 -> 10 1001 -> HL
            addToHL(read16BitRegister((opcode >> 4) & 0b11));
            return true;
        }
        // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            addToHL(stackPointer);
            return true;
        }
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // sub a,reg
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: // sbc a,reg
        {
            debug_print_4("sub reg,reg\n");
            bool carry = (opcode & 8) && isCarry();
            sbc(registers[opcode & 7], carry);
            return true;
        }
        // sub A,d8
        case 0xD6: {
            debug_print_4("sub A,d8\n");
            u8 val = memory->read8BitsAt(progCounter++);
            sbc(val, false);
            return true;
        }
        // sbc A,d8
        case 0xDE: {
            debug_print_4("sbc A,d8\n");
            u8 val = memory->read8BitsAt(progCounter++);
            sbc(val, isCarry());
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
            u16 address = memory->read16BitsAt(progCounter);
            progCounter += 2;
            if ((!set && !isZero()) || (set && isZero())) {
                debug_print_4("call from 0x%04X", progCounter - 2);
                push16Bits(progCounter);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            bool set = opcode & 0b00001000;
            debug_print_4("call (N)C,a16 set ? %d %d\n", set, isCarry());
            u16 address = memory->read16BitsAt(progCounter);
            progCounter += 2;
            if ((!set && !isCarry()) || (set && isCarry())) {
                debug_print_4("call from 0x%04X", progCounter - 2);
                push16Bits(progCounter);
                progCounter = address;
                debug_print_4(" to 0x%04X\n", progCounter);
            }
            return true;
        }
        case 0xCD: {
            u16 address = memory->read16BitsAt(progCounter);
            progCounter += 2;
            debug_print_4("call from 0x%04X\n", progCounter);
            push16Bits(progCounter);
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
            return doPrefix(memory->read8BitsAt(progCounter++));
        }
        default: {
            unknown_instr:
            fprintf(stderr,"Illegal instruction 0x%02X at 0x%04X\n", opcode, progCounter - 1);
            return false;
        }
    }
}

bool CPU::doPrefix(u8 prefix) {
    switch(prefix) {
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
            if (isCarry()) {
                newVal |= 0b10000000;
            }
            setFlags(newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
        // rr (HL)
        case 0x1E: {
            u8 oldVal = memory->read16BitsAt(readHL());
            u8 newVal = oldVal >> 1;
            if (isCarry()) {
                newVal |= 0b10000000;
            }
            setFlags(newVal == 0, false, false, oldVal & 0b1);
            memory->write8BitsTo(readHL(), newVal);
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
            setFlags(newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
        // srl (HL)
        case 0x3E: {
            u8 oldVal = memory->read16BitsAt(readHL());
            u8 newVal = oldVal >> 1;
            setFlags(newVal == 0, false, false, oldVal & 0b1);
            memory->write8BitsTo(readHL(), newVal);
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

            u8 bitMask = (prefix >> 3) & 0b111;
            u8 regPos = prefix & 0b111;
            // Note: We write the opposite of the Nth bit into the Z flag
            setFlags(!(read16BitRegister(regPos) & bitMask), false, true, isCarry());
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
            u8 bitMask = (prefix >> 3) & 0b111;
            // Note: We write the opposite of the Nth bit into the Z flag
            setFlags(!(memory->read8BitsAt(readHL()) & bitMask), false, true, isCarry());
            return true;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", prefix, progCounter - 1);
            return false;
        }
    }
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

inline void CPU::sbc(u8 val, bool carry) {
    if (carry) val++;
    u8 newVal = *regA - val;
    setFlags(newVal == 0, true, (*regA & 0xF) - (val & 0xF) < 0, *regA < val);
    *regA = newVal;
}

inline void CPU::addToHL(u16 val) {
    u16 oldVal = readHL();
    u16 newVal = oldVal + val;

    // TODO: Improve this by using the correct masks directly to calculate half-carry:
    u8 oldValMsb = (oldVal >> 8) & 0xff;
    u8 valMsb = (val >> 8) & 0xff;

    setFlags(isZero(), false, ((oldValMsb & 0xf) + (valMsb & 0xf)) > 0xf, (oldVal & 0xffff) + (val & 0xffff) > 0xffff);

    writeHL(newVal);
}