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

#define FB_CAST_8_TO_16_BIT(x) static_cast<u16*>(static_cast<void*>(x))

using namespace FunkyBoy;

CPU::CPU(std::shared_ptr<Cartridge> cartridge): progCounter(0), stackPointer(0), cartridge(std::move(cartridge)) {
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

inline bool CPU::isCarry() {
    return *regF & 0b00010000;
}

inline bool CPU::isHalfCarry() {
    return *regF & 0b00100000;
}

inline bool CPU::isSubstraction() {
    return *regF & 0b01000000;
}

inline bool CPU::isZero() {
    return *regF & 0b10000000;
}

inline void addWithCarry(u8 &a, u8 &s, bool carry) {
    a = carry ? (a + s + 1) : (a + s);
}

bool CPU::doTick() {
    // Useful resources:
    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    // http://www.devrs.com/gb/files/opcodes.html
    // https://www.reddit.com/r/EmuDev/comments/7ljc41/how_to_algorithmically_parse_gameboy_opcodes/
    // https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header
    // https://gbdev.gg8.se/wiki/articles/Gameboy_ROM_Header_Info

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
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            bool set = opcode & 0b00001000;
            if ((!set && !isZero()) || (set && isZero())) {
                goto jump_static;
            }
            return true;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            bool set = opcode & 0b00001000;
            if ((!set && !isCarry()) || (set && isCarry())) {
                goto jump_static;
            }
            return true;
        }
        // unconditional jp
        case 0xC3:
        {
jump_static:
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
        default:
            std::cerr << "Illegal instruction: " << (opcode & 0xff) << std::endl;
            return false;
    }
}

void CPU::setProgramCounter(u16 offset) {
    progCounter = offset;
}