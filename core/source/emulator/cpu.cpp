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
#include <util/registers.h>
#include <cmath>

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
#include <iomanip>
#endif

#define FB_REG_DIV 0xFF04
#define FB_REG_TIMA 0xFF05
#define FB_REG_TMA 0xFF06
#define FB_REG_TAC 0xFF07

using namespace FunkyBoy;

CPU::CPU(std::shared_ptr<Memory> memory): memory(std::move(memory)) , interruptMasterEnable(IMEState::DISABLED)
    , cpuState(CPUState::RUNNING), timerCounter(0), divCounter(0)
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

    instrContext.registers = registers;
    instrContext.regB = regB;
    instrContext.regC = regC;
    instrContext.regD = regD;
    instrContext.regE = regE;
    instrContext.regH = regH;
    instrContext.regL = regL;
    instrContext.regF = regF_do_not_use_directly;
    instrContext.regA = regA;
    instrContext.progCounter = 0;
    instrContext.stackPointer = 0xFFFE;
    instrContext.memory = this->memory;

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

    instrContext.stackPointer = 0xFFFE;

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

GameBoyType CPU::getType() {
    // TODO: Implement
    return GameBoyType::GameBoyDMG;
}

bool CPU::doTick() {
    bool result;

    if (cpuState == CPUState::RUNNING || cpuState == CPUState::HALTED) {
        if (doInterrupts()) {
            // An interrupt has started, so we set the state to RUNNING again
            cpuState = CPUState::RUNNING;
        }
    }

    // TODO: If STOPPED, react to joypad input

    // TODO: Handle timer here or earlier?
    doDivider();
    doTimer();

    if (cpuState == CPUState::RUNNING) {
        auto opcode = memory->read8BitsAt(progCounter++);
        debug_print_4("> instr 0x%02X at 0x%04X\n", opcode, progCounter - 1);

        if (opcode == 0xCB) {
            result = doPrefix(memory->read8BitsAt(progCounter++));
        } else {
            result = doInstruction(opcode);
        }
    } else {
        result = true;
    }

    // Due to a hardware quirk, enabling interrupts becomes active after the instruction following an EI,
    // so we simulate this behaviour here
    switch(interruptMasterEnable) {
        case REQUEST_ENABLE:
            interruptMasterEnable = IMEState::ENABLING;
            break;
        case ENABLING:
            interruptMasterEnable = IMEState::ENABLED;
            break;
        default:
            break;
    }

    return result;
}

void CPU::doFetch() {
    instrContext.instr = memory->read8BitsAt(instrContext.progCounter++);
}

bool CPU::doDecode() {
    switch (instrContext.instr) {
        // nop
        case 0x00:
            operands[0] = Instructions::nop;
            operands[1] = nullptr;
            return true;
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            debug_print_4("ld reg,reg\n");
            operands[0] = Instructions::loadRS;
            operands[1] = nullptr;
            return true;
        }
        // ld (a16),A
        case 0xEA: {
            debug_print_4("ld (a16),A\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::load_mem_dd_A;
            operands[3] = nullptr;
            return true;
        }
        // ld A,(a16)
        case 0xFA: {
            debug_print_4("ld A,(a16)\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::load_A_mem_dd;
            operands[3] = nullptr;
            return true;
        }
        // ld (C),A
        case 0xE2: {
            debug_print_4("ld (C),A\n");
            operands[0] = Instructions::readRegCAsLSB;
            operands[1] = Instructions::load_dd_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(C)
        case 0xF2: {
            debug_print_4("ld A,(C)\n");
            operands[0] = Instructions::readRegCAsLSB;
            operands[1] = Instructions::load_A_mem_dd;
            operands[2] = nullptr;
            return true;
        }
        // ld A,d8
        case 0x3E: {
            debug_print_4("ld A,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::load_A_d;
            operands[2] = nullptr;
            return true;
        }
        // ld (ss),d16
        case 0x01: case 0x11: case 0x21: {
            debug_print_4("ld (ss),d16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::load_dd_nn;
            operands[3] = nullptr;
            return true;
        }
        // ld SP,d16
        case 0x31: {
            debug_print_4("ld SP,d16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::load_SP_nn;
            operands[3] = nullptr;
            return true;
        }
        // ld (a16),SP
        case 0x08: {
            debug_print_4("ld (a16),SP\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::load_nn_SP;
            operands[3] = nullptr;
            return true;
        }
        // ld r,d8
        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print_4("ld r,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::load_r_n;
            operands[2] = nullptr;
            return true;
        }
        // ld (HL),d8
        case 0x36: {
            debug_print_4("ld (HL),d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::load_HL_n;
            operands[2] = nullptr;
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            operands[0] = Instructions::load_reg_dd_A;
            operands[1] = nullptr;
            return true;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            operands[0] = Instructions::load_A_reg_dd;
            operands[1] = nullptr;
            return true;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            operands[0] = Instructions::load_HLI_A;
            operands[1] = nullptr;
            return true;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            operands[0] = Instructions::load_HLD_A;
            operands[1] = nullptr;
            return true;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            operands[0] = Instructions::load_A_HLI;
            operands[1] = nullptr;
            return true;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            operands[0] = Instructions::load_A_HLD;
            operands[1] = nullptr;
            return true;
        }
        // ld (HL),r
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            debug_print_4("ld (HL),r\n");
            operands[0] = Instructions::load_HL_r;
            operands[1] = nullptr;
            return true;
        }
        // ld r,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            debug_print_4("ld r,(HL)\n");
            operands[0] = Instructions::load_r_HL;
            operands[1] = nullptr;
            return true;
        }
        // ld SP,HL
        case 0xF9: {
            debug_print_4("ld SP,HL\n");
            operands[0] = Instructions::load_SP_HL;
            operands[1] = nullptr;
            return true;
        }
        // ld HL,SP+e8
        case 0xF8: {
            debug_print_4("ld HL,SP+e8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = Instructions::load_HL_SPe;
            operands[2] = nullptr;
            return true;
        }
        // ldh (a8),A
        case 0xE0: {
            debug_print_4("ldh (a8),A\n");
            operands[0] = Instructions::readMemAsLSB;
            operands[1] = Instructions::load_mem_dd_A;
            operands[2] = nullptr;
            return true;
        }
        // ldh A,(a8)
        case 0xF0: {
            debug_print_4("ldh A,(a8)\n");
            operands[0] = Instructions::readMemAsLSB;
            operands[1] = Instructions::load_A_mem_dd;
            operands[2] = nullptr;
            return true;
        }
        // add a,reg
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: {
            debug_print_4("add A,r\n");
            operands[0] = Instructions::add_A_r;
            operands[1] = nullptr;
            return true;
        }
        // adc a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: {
            debug_print_4("adc A,r\n");
            operands[0] = Instructions::adc_A_r;
            operands[1] = nullptr;
            return true;
        }
        // add A,d8
        case 0xC6: {
            debug_print_4("add A,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::add_A_d;
            operands[2] = nullptr;
            return true;
        }
        // adc A,d8
        case 0xCE: {
            debug_print_4("add A,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::adc_A_d;
            operands[2] = nullptr;
            return true;
        }
        // add HL,ss
        case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            operands[0] = Instructions::add_HL_ss;
            operands[1] = nullptr;
            return true;
        }
        // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            operands[0] = Instructions::add_HL_SP;
            operands[1] = nullptr;
            return true;
        }
        // add SP,r8
        case 0xE8: {
            debug_print_4("add SP,e8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = Instructions::add_SP_e;
            operands[2] = nullptr;
            return true;
        }
        // add A,(HL)
        case 0x86: {
            debug_print_4("add A,(HL)\n");
            operands[0] = Instructions::add_A_HL;
            operands[1] = nullptr;
            return true;
        }
        // adc A,(HL)
        case 0x8E: {
            debug_print_4("adc A,(HL)\n");
            operands[0] = Instructions::adc_A_HL;
            operands[1] = nullptr;
            return true;
        }
    }
}

bool CPU::doInstruction(FunkyBoy::u8 opcode) {
    instrContext.instr = opcode;

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
        // sub (HL)
        case 0x96: {
            sbc(memory->read8BitsAt(readHL()), false);
            return true;
        }
        // sbc (HL)
        case 0x9E: {
            sbc(memory->read8BitsAt(readHL()), isCarry());
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            bool set = opcode & 0b00001000;
            u16 address = memory->read16BitsAt(progCounter);
            progCounter += 2;
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
            u16 address = memory->read16BitsAt(progCounter);
            progCounter += 2;
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
        // dec ss
        case 0x0B: case 0x1B: case 0x2B: {
            u8 position = opcode >> 4 & 3;
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
            setFlags(false, false, false, a & 1);
            return true;
        }
        // rlca
        case 0x07: {
            u8 a = *regA;
            *regA = (a << 1) | ((a & 128) >> 7);
            setFlags(false, false, false, (a & 128) != 0);
            return true;
        }
        // rra
        case 0x1F: {
            u8 a = *regA;
            *regA = a >> 1;
            if (isCarry()) {
                *regA |= 128; // (bit 7 set to 1)
            }
            setFlags(false, false, false, a & 1);
            return true;
        }
        // rla
        case 0x17: {
            u8 a = *regA;
            *regA = a << 1;
            if (isCarry()) {
                *regA |= 1; // (bit 0 set to 1)
            }
            setFlags(false, false, false, (a & 128) != 0);
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
            interruptMasterEnable = IMEState::ENABLED; // Immediate enable without delay
            progCounter = pop16Bits();
            return true;
        }
        // daa
        case 0x27: {
            u8 val = *regA;
            if (isSubstraction()) {
                if (isCarry()) {
                    val -= 0x60;
                }
                if (isHalfCarry()) {
                    val -= 0x06;
                }
            } else {
                if (isCarry() || val > 0x99) {
                    val += 0x60;
                    setCarry(true);
                }
                if (isHalfCarry() || (val & 0xf) > 0x09) {
                    val += 0x06;
                }
            }
            *regA = val;
            setZero(val == 0);
            setHalfCarry(false);
            return true;
        }
        // cpl
        case 0x2F: {
            *regA = ~*regA;
            setSubstraction(true);
            setHalfCarry(true);
            return true;
        }
        // scf
        case 0x37: {
            setFlags(isZero(), false, false, true);
            return true;
        }
        // ccf
        case 0x3F: {
            setFlags(isZero(), false, false, !isCarry());
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
            } else if (getType() == GameBoyType::GameBoyCGB) {
                // On non-GBC devices, the next instruction is skipped if HALT was requested with IME being disabled
                progCounter++;
            }
            return true;
        }
        case 0xCB: {
            // This should already be handled in onTick, so we should never reach this case here
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
        // rlc reg
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg << 1) | ((*reg >> 7) & 0b1);
            setFlags(newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
        // rlc (HL)
        case 0x06: {
            u8 oldVal = memory->read8BitsAt(readHL());
            u8 newVal = (oldVal << 1) | ((oldVal >> 7) & 0b1);
            setFlags(newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory->write8BitsTo(readHL(), newVal);
            return true;
        }
        // rrc reg
        case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg >> 1) | ((*reg & 0b1) << 7);
            setFlags(newVal == 0, false, false, (*reg & 0b1) > 0);
            *reg = newVal;
            return true;
        }
        // rrc (HL)
        case 0x0E: {
            u8 oldVal = memory->read8BitsAt(readHL());
            u8 newVal = (oldVal >> 1) | ((oldVal & 0b1) << 7);
            setFlags(newVal == 0, false, false, (oldVal & 0b1) > 0);
            memory->write8BitsTo(readHL(), newVal);
            return true;
        }
        // rl reg
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg << 1);
            if (isCarry()) {
                newVal |= 0b1;
            }
            setFlags(newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
        // rl (HL)
        case 0x16: {
            u8 oldVal = memory->read8BitsAt(readHL());
            u8 newVal = (oldVal << 1);
            if (isCarry()) {
                newVal |= 0b1;
            }
            setFlags(newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory->write8BitsTo(readHL(), newVal);
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
            setFlags(newVal == 0, false, false, (*reg & 0b10000000) > 0);
            *reg = newVal;
            return true;
        }
        // sla (HL)
        case 0x26: {
            u8 oldVal = memory->read16BitsAt(readHL());
            u8 newVal = oldVal << 1;
            setFlags(newVal == 0, false, false, (oldVal & 0b10000000) > 0);
            memory->write8BitsTo(readHL(), newVal);
            return true;
        }
        // sra reg
        case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F: {
            u8 *reg = registers + (prefix & 0b111);
            u8 newVal = (*reg >> 1) | (*reg & 0b10000000);
            setFlags(newVal == 0, false, false, *reg & 0b1);
            *reg = newVal;
            return true;
        }
        // sra (HL)
        case 0x2E: {
            u8 oldVal = memory->read8BitsAt(readHL());
            u8 newVal = (oldVal >> 1) | (oldVal & 0b10000000);
            setFlags(newVal == 0, false, false, oldVal & 0b1);
            memory->write8BitsTo(readHL(), newVal);
            return true;
        }
        // swap reg
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: {
            u8 *reg = registers + (prefix & 0b111);
            *reg = ((*reg >> 4) & 0b1111) | ((*reg & 0b1111) << 4);
            setFlags(*reg == 0, false, false, false);
            return true;
        }
        // swap (HL)
        case 0x36: {
            u8 val = memory->read8BitsAt(readHL());
            val = ((val >> 4) & 0b1111) | ((val & 0b1111) << 4);
            memory->write8BitsTo(readHL(), val);
            setFlags(val == 0, false, false, false);
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

            u8 bitShift = (prefix >> 3) & 0b111;
            u8 bitMask = 1 << bitShift;
            u8 regPos = prefix & 0b111;
            u8 *reg = registers + regPos;
            // Note: We write the opposite of the Nth bit into the Z flag
            setFlags(!(*reg & bitMask), false, true, isCarry());
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
            setFlags(!(memory->read8BitsAt(readHL()) & bitMask), false, true, isCarry());
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
            u8 val = memory->read8BitsAt(readHL());
            val &= ~(1 << bitShift);
            memory->write8BitsTo(readHL(), val);
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
            u8 val = memory->read8BitsAt(readHL());
            val |= (1 << bitMask);
            memory->write8BitsTo(readHL(), val);
            return true;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", prefix, progCounter - 1);
            return false;
        }
    }
}

inline memory_address getInterruptStartAddress(InterruptType type) {
    // VBLANK   -> 0x0040
    // LCD_STAT -> 0x0048
    // TIMER    -> 0x0050
    // SERIAL   -> 0x0058
    // JOYPAD   -> 0x0060
    return 0x0040 + (static_cast<u8>(type) * 0x8);
}

inline u8 getInterruptBitMask(InterruptType type) {
    return 1u << static_cast<u8>(type);
}

bool CPU::doInterrupts() {
    if (interruptMasterEnable != IMEState::ENABLED) {
        return false;
    }
    u8 _if = memory->read8BitsAt(0xFF0F) & 0b11111u;
    u8 _ie = memory->read8BitsAt(0xFFFF);
    u8 _intr = _if & _ie;
    if (!_intr) {
        return false;
    }
    for (u8 shift = 0 ; shift <= 4 ; shift++) {
        u8 bitMask = 1u << shift;
        if (!(_intr & bitMask)) {
            continue;
        }
        auto interruptType = static_cast<InterruptType>(shift);
        memory_address addr = getInterruptStartAddress(interruptType);
        interruptMasterEnable = IMEState::DISABLED;
        // TODO: do 2 NOP cycles (when implementing cycle accuracy)
        push16Bits(progCounter);
        progCounter = addr;
        debug_print_2("Do interrupt at 0x%04X\n", addr);
        _if &= ~bitMask;
        memory->write8BitsTo(0xFF0F, _if); // TODO: Investigate "Timer doesn't work" error from ROM output
        // TODO: Interrupt Service Routine should take 5 cycles
        return true;
    }
    return false;
}

void CPU::doDivider() {
    if (divCounter++ % 256 != 0) {
        // Frequency: 16384Hz => every 256th cycle
        return;
    }
    divCounter = 0;
    memory->incrementAt(FB_REG_DIV);
}

void CPU::doTimer() {
    u8 tac = memory->read8BitsAt(FB_REG_TAC);
    if (!(tac & 0b100u)) {
        return;
    }
    tac &= 0b11u;
    u16 inputClock;
    if (tac == 0) {
        inputClock = 1024;
    } else {
        inputClock = 16 * pow(4, tac - 1);
    }
    timerCounter = (timerCounter + 1) % inputClock;
    if (timerCounter != 0) {
        return;
    }
    u8 tima = memory->read8BitsAt(FB_REG_TIMA);
    bool triggerInterrupt = false;
    if (tima == 0xff) {
        triggerInterrupt = true;
        tima = memory->read8BitsAt(FB_REG_TMA);
    } else {
        tima++;
    }
    memory->write8BitsTo(FB_REG_TIMA, tima);
    if (triggerInterrupt) {
        //fprintf(stdout, "# req timer int\n");
        requestInterrupt(InterruptType::TIMER);
    }
}

void CPU::requestInterrupt(InterruptType type) {
    //fprintf(stdout, "#req int %d\n", type);
    u8 _if = memory->read8BitsAt(0xFF0F);
    memory->write8BitsTo(0xFF0F, _if | getInterruptBitMask(type));
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
    return Util::composeHL(*regH, *regL);
}

void CPU::writeHL(FunkyBoy::u16 val) {
    Util::writeHL(*regH, *regL, val);
}

u16 CPU::readAF() {
    return (*regF_do_not_use_directly & 0b11110000) | (*regA << 8);
}

void CPU::writeAF(FunkyBoy::u16 val) {
    *regF_do_not_use_directly = val & 0b11110000; // Only the 4 most significant bits are written to register F
    *regA = (val >> 8) & 0xff;
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

inline void CPU::sbc(u8 val, bool carry) {
    u8 carryVal = carry ? 1 : 0;
    u8 newVal = *regA - val - carryVal;
    setFlags(newVal == 0, true, (*regA & 0xF) - (val & 0xF) - carryVal < 0, *regA < (val + carryVal));
    *regA = newVal;
}