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
#include <util/debug.h>

using namespace FunkyBoy;

CPU::CPU(std::shared_ptr<Memory> memory): progCounter(0), stackPointer(0), memory(std::move(memory)) {
    regB = registers;
    regC = registers + 1;
    regD = registers + 2;
    regE = registers + 3;
    regH = registers + 4;
    regL = registers + 5;
    regA = registers + 6;
    regF = registers + 7;

    reg16bit = FB_CAST_8_TO_16_BIT(registers);

    regBC = FB_CAST_8_TO_16_BIT(regB);
    regDE = FB_CAST_8_TO_16_BIT(regD);
    regHL = FB_CAST_8_TO_16_BIT(regH);
    regAF = FB_CAST_8_TO_16_BIT(regA);
}

inline bool CPU::isCarry() {
    return *regF & 0b00010000;
}

void CPU::setCarry(bool carry) {
    if (carry) {
        *regF |= 0b00010000;
    } else {
        *regF &= 0b11101111;
    }
}

inline bool CPU::isHalfCarry() {
    return *regF & 0b00100000;
}

void CPU::setHalfCarry(bool halfCarry) {
    if (halfCarry) {
        *regF |= 0b00100000;
    } else {
        *regF &= 0b11011111;
    }
}

inline bool CPU::isSubstraction() {
    return *regF & 0b01000000;
}

void CPU::setSubstraction(bool substration) {
    if (substration) {
        *regF |= 0b01000000;
    } else {
        *regF &= 0b10111111;
    }
}

inline bool CPU::isZero() {
    return *regF & 0b10000000;
}

void CPU::setZero(bool zero) {
    if (zero) {
        *regF |= 0b10000000;
    } else {
        *regF &= 0b01111111;
    }
}

void CPU::setFlags(bool zero, bool subtraction, bool halfCarry, bool carry) {
    if (zero) {
        *regF |= 0b10000000;
    } else {
        *regF &= 0b01111111;
    }
    if (subtraction) {
        *regF |= 0b01000000;
    } else {
        *regF &= 0b10111111;
    }
    if (halfCarry) {
        *regF |= 0b00100000;
    } else {
        *regF &= 0b11011111;
    }
    if (carry) {
        *regF |= 0b00010000;
    } else {
        *regF &= 0b11101111;
    }
}

inline void addWithCarry(u8 &a, u8 &s, bool carry) {
    a = carry ? (a + s + 1) : (a + s);
}

bool CPU::doTick() {
    auto currOffset = progCounter;
    auto opcode = memory->read8BitsAt(progCounter++);

    debug_print("> instr 0x%02X at 0x%04X\n", opcode, currOffset);

    switch (opcode) {
        // nop
        case 0x00:
            debug_print("nop\n");
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
            debug_print("ld reg,reg\n");
            u8 &dst = registers[opcode >> 3 & 7];
            u8 src = registers[opcode & 7];
            dst = src;
            return true;;
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
            debug_print("ldh A,d8 A <- 0x%02X\n", *regA);
            return true;
        }
        // ld (ss),d16
        case 0x01: case 0x11: case 0x21: {
            debug_print("ld (ss),d16\n");
            *(reg16bit + (opcode >> 4 & 3)) = memory->read16BitsAt(progCounter);
            progCounter += 2;
            return true;
        }
        // ld SP,d16
        case 0x31: {
            debug_print("ld SP,d16\n");
            stackPointer = memory->read16BitsAt(progCounter); // TODO: Correct?
            progCounter += 2;
            return true;
        }
        // ld (a16),SP
        case 0x08: {
            debug_print("ld (a16),SP\n");
            memory->write16BitsTo(memory->read16BitsAt(progCounter), stackPointer);
            progCounter += 2;
            return true;
        }
        // ld s,d8
        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print("ld s,d8\n");
            *(registers + (opcode >> 3 & 5)) = memory->read8BitsAt(progCounter++);
            return true;
        }
        // ld (HL),d8
        case 0x36: {
            debug_print("ld (HL),d8\n");
            memory->write8BitsTo(*regHL, memory->read8BitsAt(progCounter++)); // TODO: Correct?
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print("ld (ss),A\n");
            memory->write8BitsTo(*(reg16bit + (opcode >> 4 & 1)), *regA);
            return true;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print("ld A,(ss)\n");
            *regA = memory->read8BitsAt(*(reg16bit + (opcode >> 4 & 1)));
            return true;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print("ld (HLI),A\n");
            memory->write8BitsTo(*regHL, *regA);
            (*regHL)++;
            return true;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print("ld (HLD),A\n");
            memory->write8BitsTo(*regHL, *regA);
            (*regHL)--;
            return true;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print("ld A,(HLI)\n");
            *regA = memory->read8BitsAt(*regHL);
            (*regHL)++;
            return true;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print("ld A,(HLD)\n");
            *regA = memory->read8BitsAt(*regHL);
            (*regHL)--;
            return true;
        }
        // ldh (a8),A
        case 0xE0: {
            auto addr = memory->read8BitsAt(progCounter++);
            debug_print("ldh (a8),A 0x%04X <- 0x%02X\n", 0xFF00 + addr, *regA);
            memory->write8BitsTo(0xFF00 + addr, *regA);
            return true;
        }
        // ldh A,(a8)
        case 0xF0: {
            auto addr = memory->read8BitsAt(progCounter++);
            *regA = memory->read8BitsAt(0xFF00 + addr);
            return true;
        }
        // add reg,reg TODO: Correct notation?
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            debug_print("add reg,reg\n");
            bool carry = (opcode & 8) && isCarry();
            addWithCarry(*regA, registers[opcode & 7], carry);
            setCarry(false);
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            bool set = opcode & 0b00001000;
            if ((!set && !isZero()) || (set && isZero())) {
                goto jump_absolute;
            }
            return true;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            bool set = opcode & 0b00001000;
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto jump_absolute;
            }
            return true;
        }
        // unconditional jp
        case 0xC3:
        {
jump_absolute:
            debug_print("jp a16 from 0x%04X", progCounter);
            progCounter = memory->read16BitsAt(progCounter);
            debug_print(" to 0x%04X\n", progCounter);
            return true;
        }
        // jp AF
        case 0xE9:
        {
            debug_print("jp AF\n");
            progCounter = *regAF;
            return true;
        }
        // jr (N)Z,r8
        case 0x20: case 0x28: { // TODO: Can this branch bew combined with jp (N)Z,a16 ?
            bool set = opcode & 0b00001000;
            debug_print("jr (N)Z,r8 set ? %d %d\n", set, isZero());
            if ((!set && !isZero()) || (set && isZero())) {
                goto jump_relative;
            }
            return true;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: { // TODO: Can this branch bew combined with jp (N)C,a16 ?
            bool set = opcode & 0b00001000;
            debug_print("jr (N)C,r8 set ? %d %d\n", set, isCarry());
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto jump_relative;
            }
            return true;
        }
        // unconditional jr
        case 0x18: {
            debug_print("jr\n");
jump_relative:
            debug_print("JR from 0x%04X", progCounter);
            auto signedByte = static_cast<i8>(memory->read8BitsAt(progCounter++)); // TODO: Verify that we should increment here, but it seems logical
            progCounter += signedByte;
            debug_print(" to 0x%04X\n", progCounter);
            return true;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            bool set = opcode & 0b00001000;
            debug_print("call (N)Z,a16 set ? %d %d\n", set, isZero());
            if ((!set && !isZero()) || (set && isZero())) {
                goto call;
            }
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            bool set = opcode & 0b00001000;
            debug_print("call (N)C,a16 set ? %d %d\n", set, isCarry());
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto call;
            }
            return true;
        }
        case 0xCD: {
call:
            debug_print("call from 0x%04X", progCounter);
            u16 val = memory->read16BitsAt(progCounter);
            push16Bits(progCounter + 2);
            progCounter = val;
            debug_print(" to 0x%04X\n", progCounter);
            return true;
        }
        // ret (N)Z,a16
        case 0xC0: case 0xC8: {
            bool set = opcode & 0b00001000;
            debug_print("ret (N)Z,a16 set ? %d %d\n", set, isZero());
            if ((!set && !isZero()) || (set && isZero())) {
                goto return_;
            }
            return true;
        }
        // ret (N)C,a16
        case 0xD0: case 0xD8: {
            bool set = opcode & 0b00001000;
            debug_print("ret (N)C,a16 set ? %d %d\n", set, isCarry());
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto return_;
            }
            return true;
        }
        // ret a16
        case 0xC9: {
            debug_print("ret a16\n");
return_:
            progCounter = pop16Bits();
            return true;
        }
        // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            u8 rstAddr = (opcode >> 3 & 7) * 8;
            debug_print("rst %02XH\n", rstAddr);
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
            cp(memory->read8BitsAt(*regHL));
            return true;
        }
        // cp d8
        case 0xFE: {
            cp(memory->read8BitsAt(progCounter++));
            return true;
        }
        // inc ss
        case 0x03: case 0x13: case 0x23: {
            *(reg16bit + (opcode >> 4 & 3)) += 1;
            return true;
        }
        // inc SP
        case 0x33: {
            stackPointer--; // TODO: Is this correct?
            return true;
        }
        // inc (HL)
        case 0x34: {
            setHalfCarry(((memory->read8BitsAt(*regHL) & 0xf) + (1 & 0xf)) & 0x10);
            memory->incrementAt(*regHL);
            setZero(memory->read8BitsAt(*regHL) == 0);
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // inc s
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: {
            auto reg = registers + (opcode >> 3 & 7);
            setHalfCarry(((*reg & 0xf) + (1 & 0xf)) & 0x10);
            (*reg)++;
            setZero(*reg == 0);
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // inc A
        case 0x3C: {
            setHalfCarry(((*regA & 0xf) + (1 & 0xf)) & 0x10);
            (*regA)++;
            setZero(*regA == 0);
            setSubstraction(false);
            // Leave carry as-is
            return true;
        }
        // dec (HL)
        case 0x35: {
            setHalfCarry(((memory->read8BitsAt(*regHL) & 0xf) - (1 & 0xf)) & 0x10);
            memory->decrementAt(*regHL);
            setZero(memory->read8BitsAt(*regHL) == 0);
            setSubstraction(true);
            // Leave carry as-is
            return true;
        }
        // dec s
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: {
            auto reg = registers + (opcode >> 3 & 7);
            setHalfCarry(((*reg & 0xf) - (1 & 0xf)) & 0x10);
            (*reg)--;
            setZero(*reg == 0);
            setSubstraction(true);
            // Leave carry as-is
            return true;
        }
        // dec A
        case 0x3D: {
            setHalfCarry(((*regA & 0xf) - (1 & 0xf)) & 0x10);
            (*regA)--;
            setZero(*regA == 0);
            setSubstraction(true);
            // Leave carry as-is
            return true;
        }
        // xor s
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: {
            _xor(*(registers + (opcode & 7)));
            return true;
        }
        // xor (HL)
        case 0xAE: {
            _xor(memory->read8BitsAt(*regHL));
            return true;
        }
        // xor A
        case 0xAF: {
            _xor(*regA);
            return true;
        }
        // xor d8
        case 0xEE: {
            auto val = memory->read8BitsAt(progCounter++);
            _xor(val);
            return true;
        }
        case 0xD9: {
            // TODO: RETI instruction
            // Returns from an interrupt routine. Note: RETI cannot use return conditions.
            goto unknown_instr;
        }
        case 0xF3: {
            memory->write8BitsTo(FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER, 0);
            return true;
        }
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
    memory->write8BitsTo(stackPointer - 2, (u8) (val));
    memory->write8BitsTo(stackPointer - 1, (u8) (val >> 8u));
    stackPointer -= 2;
}

u16 CPU::pop16Bits() {
    u16 val = memory->read8BitsAt(stackPointer - 2) | (memory->read8BitsAt(stackPointer - 1) << 8u);
    stackPointer += 2;
    return val;
}

void CPU::cp(u8 val) {
    debug_print("cp 0x%02X - 0x%02X\n", *regA, val);
    // See http://z80-heaven.wikidot.com/instructions-set:cp
    setFlags(*regA == val, true, (*regA & 0xF) < (val & 0xF), *regA < val);
}

void CPU::_xor(u8 val) {
    debug_print("xor 0x%02X ^ 0x%02X\n", *regA, val);
    *regA ^= val;
    setFlags(*regA == 0, false, false, false);
}