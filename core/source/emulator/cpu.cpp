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

// TODO: For debugging, remove it afterwards:
#include <sstream>

#define FB_CAST_8_TO_16_BIT(x) static_cast<u16*>(static_cast<void*>(x))

using namespace FunkyBoy;

CPU::CPU(std::shared_ptr<Cartridge> cartridge): progCounter(0), stackPointer(0), cartridge(std::move(cartridge)) {
    ram = new u8[32768];

    regB = registers;
    regC = registers + 1;
    regD = registers + 2;
    regE = registers + 3;
    regH = registers + 4;
    regL = registers + 5;
    regA = registers + 6;
    regF = registers + 7;

    regBC = FB_CAST_8_TO_16_BIT(regB);
    regDE = FB_CAST_8_TO_16_BIT(regD);
    regHL = FB_CAST_8_TO_16_BIT(regH);
    regAF = FB_CAST_8_TO_16_BIT(regA);
}

CPU::~CPU() {
    delete[] ram;
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

inline void addWithCarry(u8 &a, u8 &s, bool carry) {
    a = carry ? (a + s + 1) : (a + s);
}

bool CPU::doTick() {
    // Useful resources:
    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    // http://www.devrs.com/gb/files/opcodes.html
    // https://www.reddit.com/r/EmuDev/comments/7ljc41/how_to_algorithmically_parse_gameboy_opcodes/
    // https://www.reddit.com/r/EmuDev/comments/4clh23/trouble_with_halfcarrycarry_flag/
    // https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header
    // https://gbdev.gg8.se/wiki/articles/Gameboy_ROM_Header_Info
    // http://z80-heaven.wikidot.com/instructions-set
    // http://tutorials.eeems.ca/Z80ASM/app1d.htm
    // http://www.z80.info/z80sflag.htm

    // TODO: Remove logs

    auto opcode = cartridge->instructionAt(progCounter++);
    switch (opcode) {
        // nop
        case 0x00:
            std::cout << "nop" << std::endl;
            break;
        // ld reg,reg
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            std::cout << "ld reg,reg" << std::endl;
            u8 &dst = registers[opcode >> 3 & 7];
            u8 src = registers[opcode & 7];
            dst = src;
            return true;;
        }
        // add reg,reg TODO: Correct notation?
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            std::cout << "add reg,reg" << std::endl;
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
            std::cout << "jp a16" << std::endl;
            progCounter = cartridge->read16BitsAt(progCounter);
            return true;
        }
        // jp AF
        case 0xE9:
        {
            std::cout << "jp AF" << std::endl;
            progCounter = *regAF;
            return true;
        }
        // jr (N)Z,r8
        case 0x20: case 0x28: { // TODO: Can this branch bew combined with jp (N)Z,a16 ?
            bool set = opcode & 0b00001000;
            if ((!set && !isZero()) || (set && isZero())) {
                goto jump_relative;
            }
            return true;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: { // TODO: Can this branch bew combined with jp (N)C,a16 ?
            bool set = opcode & 0b00001000;
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto jump_relative;
            }
            return true;
        }
        // unconditional jr
        case 0x18: {
jump_relative:
            auto rel = cartridge->instructionAt(progCounter);
            progCounter += rel;
            return true;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            bool set = opcode & 0b00001000;
            if ((!set && !isZero()) || (set && isZero())) {
                goto call;
            }
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            bool set = opcode & 0b00001000;
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto call;
            }
            return true;
        }
        case 0xCD: {
call:
            u16 val = cartridge->read16BitsAt(progCounter);
            push16Bits(progCounter + 2);
            progCounter = val;
            return true;
        }
        // ret (N)Z,a16
        case 0xC0: case 0xC8: {
            bool set = opcode & 0b00001000;
            if ((!set && !isZero()) || (set && isZero())) {
                goto return_;
            }
            return true;
        }
        // ret (N)C,a16
        case 0xD0: case 0xD8: {
            bool set = opcode & 0b00001000;
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto return_;
            }
            return true;
        }
        // ret a16
        case 0xC9: {
return_:
            progCounter = pop16Bits();
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
            cp(ram[*regHL]);
            return true;
        }
        // cp d8
        case 0xFE: {
            cp(cartridge->instructionAt(progCounter++));
            return true;
        }
        case 0xD9: {
            // TODO: RETI instruction
            // Returns from an interrupt routine. Note: RETI cannot use return conditions.
        }
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            // TODO: RST (reset) instruction
            // The long name of this instruction is Restart; it is basically a call to the given address. The action
            // taken by the rst’s depends on what kind of hardware you have. On the TI calculators they are equivalent
            // to some of the most important ROM calls. Another example from the Spectrum: rst $28 activates the built-in
            // floating point calculator. The rst instruction does not affect the flags either.
        }
        default:
unknown_instr:
            std::stringstream ss;
            ss << std::hex << static_cast<unsigned int>(opcode & 0xff);
            std::cerr << "Illegal instruction: 0x" << ss.str() << std::endl;
            return false;
    }
}

void CPU::setProgramCounter(u16 offset) {
    progCounter = offset;
}

void CPU::push16Bits(FunkyBoy::u16 val) {
    ram[stackPointer - 2] = (u8) (val);
    ram[stackPointer - 1] = (u8) (val >> 8u);
    stackPointer -= 2;
}

u16 CPU::pop16Bits() {
    u16 val = ram[stackPointer - 2] | (ram[stackPointer - 1] << 8u);
    stackPointer += 2;
    return val;
}

void CPU::cp(u8_or_16 val) {
    std::cout << "cp " << (val & 0xff) << std::endl;
    // See http://z80-heaven.wikidot.com/instructions-set:cp
    setZero(*regA == val);
    setSubstraction(true);
    setHalfCarry(((*regA & 0xF) - (val & 0xF)) < 0);
    setCarry(*regA < val);
}