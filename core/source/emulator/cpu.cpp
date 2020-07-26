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

#include <utility>
#include <util/typedefs.h>
#include <util/registers.h>
#include <emulator/io_registers.h>

using namespace FunkyBoy;

CPU::CPU(GameBoyType gbType, MemoryPtr memory, io_registers_ptr ioRegisters): memory(std::move(memory)), gbType(gbType)
    , ioRegisters(std::move(ioRegisters)), instrContext(gbType), timerOverflowingCycles(-1), delayedTIMAIncrease(false)
    , operandIndex(0), cpuInterCycleCounter(0), joypadWasNotPressed(true)
#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    , file("exec_opcodes_fb_v2.txt"), instr(0)
#endif
#if defined(FB_TESTING)
    , instructionCompleted(false)
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
    instrContext.operands = operands;
    instrContext.interruptMasterEnable = IMEState::DISABLED;
    instrContext.haltBugRequested = false;
    instrContext.cpuState = CPUState::RUNNING;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    instrContext.executionLog = &file;
#endif

    // Fetch/Execute overlapping -> initial fetch is performed without executing any other instruction
    // To simulate this, we set a NOP as the first instruction, which does nothing
    operands[0] = Instructions::nop;
    operands[1] = nullptr;

    // Initialize registers
    powerUpInit();
}

void CPU::powerUpInit() {
    // Ref: https://gbdev.io/pandocs/#power-up-sequence

    // AF -> 0x01b0
    if (gbType == GameBoyType::GameBoyCGB) {
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
    if (gbType == GameBoyDMG) {
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

    // Initialize Joypad
    memory->updateJoypad();
}

bool CPU::doTick() {
    bool result;

    if (cpuInterCycleCounter == 0) {
        doJoypad();
        result = doCycle();
    } else {
        result = true;
    }

    // TODO: Handle timer here or earlier?
    doTimers();

    // Due to a hardware quirk, enabling interrupts becomes active after the instruction following an EI,
    // so we simulate this behaviour here
    switch(instrContext.interruptMasterEnable) {
        case REQUEST_ENABLE:
            instrContext.interruptMasterEnable = IMEState::ENABLING;
            break;
        case ENABLING:
            instrContext.interruptMasterEnable = IMEState::ENABLED;
            break;
        default:
            break;
    }

    cpuInterCycleCounter = (cpuInterCycleCounter + 1) % 4;

    return result;
}

bool CPU::doCycle() {
    if (instrContext.cpuState == CPUState::STOPPED) {
        return true;
    }

    bool shouldDoInterrupts = instrContext.cpuState == CPUState::HALTED;
    bool shouldFetch = false;

    if (instrContext.cpuState == CPUState::RUNNING) {
        auto op = operands[operandIndex++];

        shouldFetch = operands[operandIndex] == nullptr;

        if (!op(instrContext)) {
            shouldFetch = true;
        }
        shouldDoInterrupts = shouldFetch;
    }

#if defined(FB_TESTING)
    if (shouldFetch || shouldDoInterrupts) {
        instructionCompleted = true;
    }
#endif

    // TODO:
    // HALT mode is exited when a flag in register IF is set and the corresponding flag in IE is also set,
    // regardless of the value of IME. The only difference is that IME='1' will make the CPU jump to the
    // interrupt vector (and clear the IF flag), while IME='0' will only make the CPU continue executing
    // instructions, but the jump won't be performed (and the IF flag won't be cleared).

    if (shouldDoInterrupts && (instrContext.cpuState == CPUState::RUNNING || instrContext.cpuState == CPUState::HALTED) && doInterrupts()) {
        // An interrupt has started (or IME=false && (IF & IE) != 0), so we set the state to RUNNING (again)
        instrContext.cpuState = CPUState::RUNNING;
    }

    if (shouldFetch) {
        bool result = doFetchAndDecode();
        operandIndex = 0;
        return result;
    } else {
        return true;
    }
}

bool CPU::doFetchAndDecode() {
    if (!instrContext.haltBugRequested) {
        instrContext.instr = memory->read8BitsAt(instrContext.progCounter++);
    } else {
        instrContext.instr = memory->read8BitsAt(instrContext.progCounter);
        instrContext.haltBugRequested = false;
    }

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    FunkyBoy::Debug::writeExecutionToLog('I', file, instrContext);
    instr++;
#endif

    switch (instrContext.instr) {
        // nop
        case 0x00: {
            debug_print_4("nop\n");
            operands[0] = Instructions::nop;
            operands[1] = nullptr;
            return true;
        }
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            debug_print_4("ld reg,reg\n");
            operands[0] = Instructions::load_r_r;
            operands[1] = nullptr;
            return true;
        }
        // ld (a16),A
        case 0xEA: {
            debug_print_4("ld (a16),A\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::_pad_; // TODO: do something useful here
            operands[3] = Instructions::load_mem_dd_A;
            operands[4] = nullptr;
            return true;
        }
        // ld A,(a16)
        case 0xFA: {
            debug_print_4("ld A,(a16)\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::_pad_; // TODO: do something useful here
            operands[3] = Instructions::load_A_mem_dd;
            operands[4] = nullptr;
            return true;
        }
        // ld (C),A
        case 0xE2: {
            debug_print_4("ld (C),A\n");
            operands[0] = Instructions::readRegCAsLSB;
            operands[1] = Instructions::load_mem_dd_A;
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
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[2] = Instructions::_pad_;
            operands[3] = Instructions::_pad_;
            //
            operands[4] = Instructions::load_nn_SP;
            operands[5] = nullptr;
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
            operands[1] = Instructions::_pad_; // TODO: do something useful here
            operands[2] = Instructions::load_HL_n;
            operands[3] = nullptr;
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_reg_dd_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_A_reg_dd;
            operands[2] = nullptr;
            return true;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_HLI_A;
            operands[2] = nullptr;
            return true;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_HLD_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_A_HLI;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_A_HLD;
            operands[2] = nullptr;
            return true;
        }
        // ld (HL),r
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            debug_print_4("ld (HL),r\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_HL_r;
            operands[2] = nullptr;
            return true;
        }
        // ld r,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            debug_print_4("ld r,(HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_r_HL;
            operands[2] = nullptr;
            return true;
        }
        // ld SP,HL
        case 0xF9: {
            debug_print_4("ld SP,HL\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::load_SP_HL;
            operands[2] = nullptr;
            return true;
        }
        // ld HL,SP+e8
        case 0xF8: {
            debug_print_4("ld HL,SP+e8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = Instructions::_pad_; // TODO: do something useful here
            operands[2] = Instructions::load_HL_SPe;
            operands[3] = nullptr;
            return true;
        }
        // ldh (a8),A
        case 0xE0: {
            debug_print_4("ldh (a8),A\n");
            operands[0] = Instructions::readMemAsLSB;
            operands[1] = Instructions::_pad_; // TODO: do something useful here
            operands[2] = Instructions::load_mem_dd_A;
            operands[3] = nullptr;
            return true;
        }
        // ldh A,(a8)
        case 0xF0: {
            debug_print_4("ldh A,(a8)\n");
            operands[0] = Instructions::readMemAsLSB;
            operands[1] = Instructions::_pad_; // TODO: do something useful here
            operands[2] = Instructions::load_A_mem_dd;
            operands[3] = nullptr;
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
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::add_HL_ss;
            operands[2] = nullptr;
            return true;
        }
        // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::add_HL_SP;
            operands[2] = nullptr;
            return true;
        }
        // add SP,r8
        case 0xE8: {
            debug_print_4("add SP,e8\n");
            operands[0] = Instructions::readSigned;
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[1] = Instructions::_pad_;
            operands[2] = Instructions::_pad_;
            //
            operands[3] = Instructions::add_SP_e;
            operands[4] = nullptr;
            return true;
        }
        // add A,(HL)
        case 0x86: {
            debug_print_4("add A,(HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::add_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // adc A,(HL)
        case 0x8E: {
            debug_print_4("adc A,(HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::adc_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // sub a,reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: {
            debug_print_4("sub A,r\n");
            operands[0] = Instructions::sub_A_r;
            operands[1] = nullptr;
            return true;
        }
        // sbc a,reg
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: {
            debug_print_4("sbc A,r\n");
            operands[0] = Instructions::sbc_A_r;
            operands[1] = nullptr;
            return true;
        }
        // sub A,d8
        case 0xD6: {
            debug_print_4("sub A,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::sub_A_d;
            operands[2] = nullptr;
            return true;
        }
        // sbc A,d8
        case 0xDE: {
            debug_print_4("sbc A,d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::sbc_A_d;
            operands[2] = nullptr;
            return true;
        }
        // sub (HL)
        case 0x96: {
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::sub_HL;
            operands[2] = nullptr;
            return true;
        }
        // sbc (HL)
        case 0x9E: {
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::sbc_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            debug_print_4("jp (N)Z,a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = (instrContext.instr == 0xC2) ? Instructions::checkIsNotZero : Instructions::checkIsZero;
            operands[3] = Instructions::jp;
            operands[4] = nullptr;
            return true;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            debug_print_4("jp (N)C,a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = (instrContext.instr == 0xD2) ? Instructions::checkIsNotCarry : Instructions::checkIsCarry;
            operands[3] = Instructions::jp;
            operands[4] = nullptr;
            return true;
        }
        // unconditional jp
        case 0xC3:
        {
            debug_print_4("jp a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = Instructions::_pad_;
            operands[3] = Instructions::jp;
            operands[4] = nullptr;
            return true;
        }
        // jp HL
        case 0xE9:
        {
            debug_print_4("jp HL\n");
            operands[0] = Instructions::jp_HL;
            operands[1] = nullptr;
            return true;
        }
        // jr (N)Z,r8
        case 0x20: case 0x28: {
            debug_print_4("jr (N)Z,r8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = (instrContext.instr == 0x20) ? Instructions::checkIsNotZero : Instructions::checkIsZero;
            operands[2] = Instructions::jr;
            operands[3] = nullptr;
            return true;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: {
            debug_print_4("jr (N)C,r8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = (instrContext.instr == 0x30) ? Instructions::checkIsNotCarry : Instructions::checkIsCarry;
            operands[2] = Instructions::jr;
            operands[3] = nullptr;
            return true;
        }
        // unconditional jr
        case 0x18: {
            debug_print_4("jr r8\n");
            operands[0] = Instructions::readSigned;
            operands[1] = Instructions::_pad_;
            operands[2] = Instructions::jr;
            operands[3] = nullptr;
            return true;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            debug_print_4("call (N)Z,a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = (instrContext.instr == 0xC4) ? Instructions::checkIsNotZero : Instructions::checkIsZero;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[3] = Instructions::_pad_;
            operands[4] = Instructions::_pad_;
            //
            operands[5] = Instructions::call;
            operands[6] = nullptr;
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            debug_print_4("call (N)C,a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            operands[2] = (instrContext.instr == 0xD4) ? Instructions::checkIsNotCarry : Instructions::checkIsCarry;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[3] = Instructions::_pad_;
            operands[4] = Instructions::_pad_;
            //
            operands[5] = Instructions::call;
            operands[6] = nullptr;
            return true;
        }
        // call a16
        case 0xCD: {
            debug_print_4("call a16\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::readMSB;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[2] = Instructions::_pad_;
            operands[3] = Instructions::_pad_;
            operands[4] = Instructions::_pad_;
            //
            operands[5] = Instructions::call;
            operands[6] = nullptr;
            return true;
        }
        // ret (N)Z
        case 0xC0: case 0xC8: {
            debug_print_4("ret (N)Z\n");
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_; // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            operands[1] = (instrContext.instr == 0xC0) ? Instructions::checkIsNotZero : Instructions::checkIsZero;
            operands[2] = Instructions::_pad_;
            operands[3] = Instructions::_pad_;
            operands[4] = Instructions::ret;
            operands[5] = nullptr;
            return true;
        }
        // ret (N)C
        case 0xD0: case 0xD8: {
            debug_print_4("ret (N)C\n");
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_; // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            operands[1] = (instrContext.instr == 0xD0) ? Instructions::checkIsNotCarry : Instructions::checkIsCarry;
            operands[2] = Instructions::_pad_;
            operands[3] = Instructions::_pad_;
            operands[4] = Instructions::ret;
            operands[5] = nullptr;
            return true;
        }
        // ret
        case 0xC9: {
            debug_print_4("ret\n");
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::_pad_;
            operands[2] = Instructions::_pad_;
            //
            operands[3] = Instructions::ret;
            operands[4] = nullptr;
            return true;
        }
        // reti
        case 0xD9: {
            debug_print_4("reti\n");
            operands[0] = Instructions::enableInterruptsImmediately;
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[1] = Instructions::_pad_;
            operands[2] = Instructions::_pad_;
            //
            operands[3] = Instructions::ret;
            operands[4] = nullptr;
            return true;
        }
        // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            debug_print_4("rst vec\n");
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::_pad_;
            operands[2] = Instructions::_pad_;
            //
            operands[3] = Instructions::rst;
            operands[4] = nullptr;
            return true;
        }
        // cp r
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
            debug_print_4("cp r\n");
            operands[0] = Instructions::cp_r;
            operands[1] = nullptr;
            return true;
        }
        // cp (HL)
        case 0xBE: {
            debug_print_4("cp (HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::cp_HL;
            operands[2] = nullptr;
            return true;
        }
        // cp d8
        case 0xFE: {
            debug_print_4("cp d8\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::cp_d;
            operands[2] = nullptr;
            return true;
        }
        // inc ss
        case 0x03: case 0x13: case 0x23: {
            debug_print_4("inc ss\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::inc_ss;
            operands[2] = nullptr;
            return true;
        }
        // inc SP
        case 0x33: {
            debug_print_4("inc SP\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::inc_SP;
            operands[2] = nullptr;
            return true;
        }
        // inc (HL)
        case 0x34: {
            debug_print_4("inc (HL)\n");
            // Pad artificially to 3 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::_pad_;
            //
            operands[2] = Instructions::inc_HL;
            operands[3] = nullptr;
            return true;
        }
        // inc r
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x3C: {
            debug_print_4("inc r\n");
            operands[0] = Instructions::inc_r;
            operands[1] = nullptr;
            return true;
        }
        // dec ss
        case 0x0B: case 0x1B: case 0x2B: {
            debug_print_4("dec ss\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::dec_ss;
            operands[2] = nullptr;
            return true;
        }
        // dec SP
        case 0x3B: {
            debug_print_4("dec SP\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::dec_SP;
            operands[2] = nullptr;
            return true;
        }
        // dec (HL)
        case 0x35: {
            debug_print_4("dec (HL)\n");
            // Pad artificially to 3 machine cycles TODO: do something useful here
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::_pad_;
            //
            operands[2] = Instructions::dec_HL;
            operands[3] = nullptr;
            return true;
        }
        // dec s
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: case 0x3D: {
            debug_print_4("dec r\n");
            operands[0] = Instructions::dec_r;
            operands[1] = nullptr;
            return true;
        }
        // or r
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: {
            debug_print_4("or r\n");
            operands[0] = Instructions::or_r;
            operands[1] = nullptr;
            return true;
        }
        // or (HL)
        case 0xB6: {
            debug_print_4("or (HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::or_HL;
            operands[2] = nullptr;
            return true;
        }
        // or d8
        case 0xF6: {
            debug_print_4("or d\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::or_d;
            operands[2] = nullptr;
            return true;
        }
        // and r
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: {
            debug_print_4("and r\n");
            operands[0] = Instructions::and_r;
            operands[1] = nullptr;
            return true;
        }
        // and (HL)
        case 0xA6: {
            debug_print_4("and (HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::and_HL;
            operands[2] = nullptr;
            return true;
        }
        // and d8
        case 0xE6: {
            debug_print_4("and d\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::and_d;
            operands[2] = nullptr;
            return true;
        }
        // xor r
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            debug_print_4("xor r\n");
            operands[0] = Instructions::xor_r;
            operands[1] = nullptr;
            return true;
        }
        // xor (HL)
        case 0xAE: {
            debug_print_4("xor (HL)\n");
            operands[0] = Instructions::_pad_; // TODO: do something useful here
            operands[1] = Instructions::xor_HL;
            operands[2] = nullptr;
            return true;
        }
        // xor d8
        case 0xEE: {
            debug_print_4("xor d\n");
            operands[0] = Instructions::readLSB;
            operands[1] = Instructions::xor_d;
            operands[2] = nullptr;
            return true;
        }
        // rrca
        case 0x0F: {
            debug_print_4("rrca\n");
            operands[0] = Instructions::rrca;
            operands[1] = nullptr;
            return true;
        }
        // rlca
        case 0x07: {
            debug_print_4("rlca\n");
            operands[0] = Instructions::rlca;
            operands[1] = nullptr;
            return true;
        }
        // rra
        case 0x1F: {
            debug_print_4("rra\n");
            operands[0] = Instructions::rra;
            operands[1] = nullptr;
            return true;
        }
        // rla
        case 0x17: {
            debug_print_4("rla\n");
            operands[0] = Instructions::rla;
            operands[1] = nullptr;
            return true;
        }
        // pop rr
        case 0xC1: case 0xD1: case 0xE1: {
            debug_print_4("pop rr\n");
            operands[0] = Instructions::readStackIntoLSB;
            operands[1] = Instructions::readStackIntoMSB;
            operands[2] = Instructions::write16BitsIntoRR;
            operands[3] = nullptr;
            return true;
        }
        // pop AF
        case 0xF1: {
            debug_print_4("pop AF\n");
            operands[0] = Instructions::readStackIntoLSB;
            operands[1] = Instructions::readStackIntoMSB;
            operands[2] = Instructions::write16BitsIntoAF;
            operands[3] = nullptr;
            return true;
        }
        // push rr
        case 0xC5: case 0xD5: case 0xE5: {
            debug_print_4("push rr\n");
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::readRRMSBIntoStack;
            operands[2] = Instructions::readRRLSBIntoStack;
            operands[3] = Instructions::_pad_;
            operands[4] = nullptr;
            return true;
        }
        // push AF
        case 0xF5: {
            debug_print_4("push AF\n");
            operands[0] = Instructions::_pad_;
            operands[1] = Instructions::readRegAIntoStack;
            operands[2] = Instructions::readRegFIntoStack;
            operands[3] = Instructions::_pad_;
            operands[4] = nullptr;
            return true;
        }
        // daa
        case 0x27: {
            debug_print_4("daa\n");
            operands[0] = Instructions::daa;
            operands[1] = nullptr;
            return true;
        }
        // cpl
        case 0x2F: {
            debug_print_4("cpl\n");
            operands[0] = Instructions::cpl;
            operands[1] = nullptr;
            return true;
        }
        // scf
        case 0x37: {
            debug_print_4("scf\n");
            operands[0] = Instructions::scf;
            operands[1] = nullptr;
            return true;
        }
        // ccf
        case 0x3F: {
            debug_print_4("ccf\n");
            operands[0] = Instructions::ccf;
            operands[1] = nullptr;
            return true;
        }
        // di
        case 0xF3: {
            debug_print_4("di\n");
            operands[0] = Instructions::disableInterrupts;
            operands[1] = nullptr;
            return true;
        }
        // ei
        case 0xFB: {
            debug_print_4("ei\n");
            operands[0] = Instructions::enableInterruptsDelayed;
            operands[1] = nullptr;
            return true;
        }
        // stop
        case 0x10: {
            debug_print_4("stop\n");
            operands[0] = Instructions::stop;
            operands[1] = nullptr;
            return true;
        }
        // halt
        case 0x76: {
            debug_print_4("halt\n");
            operands[0] = Instructions::halt;
            operands[1] = nullptr;
            return true;
        }
        // prefix
        case 0xCB: {
            debug_print_4("prefix (CB)\n");
            operands[0] = Instructions::decodePrefix;
            // Small workaround:
            // To avoid our fetch/execution overlap implementation to override the prefix operands fetched in
            // Instructions::decodePrefix, we add a dummy operand. Like this when we check if the next operand would
            // have been null, we do not directly replace the prefix operands. We only do it once they have been
            // executed.
            operands[1] = Instructions::nop;
            operands[2] = nullptr;
            return true;
        }
        default: {
            fprintf(stderr, "Illegal instruction 0x%02X at 0x%04X\n", instrContext.instr, instrContext.progCounter - 1);
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

void CPU::doJoypad() {
    u8 oldP1 = memory->read8BitsAt(FB_REG_P1) & 0b00001111u;
    u8 newP1 = memory->updateJoypad() & 0b00001111u;
    bool isNotPressed = oldP1 & newP1;
    if (!isNotPressed && joypadWasNotPressed) {
        requestInterrupt(InterruptType::JOYPAD);
    }
    if (!isNotPressed && instrContext.cpuState == CPUState::STOPPED) {
        instrContext.cpuState = CPUState::RUNNING;
    }
    joypadWasNotPressed = isNotPressed;
}

bool CPU::doInterrupts() {
    if (instrContext.cpuState == CPUState::STOPPED) {
        return false;
    }
    u8 _if = memory->read8BitsAt(FB_REG_IF) & 0x1fu;
    u8 _ie = memory->read8BitsAt(FB_REG_IE) & 0x1fu;
    u8 _intr = _if & _ie & 0x1f;
    if (!_intr) {
        return false;
    }
    if (instrContext.interruptMasterEnable != IMEState::ENABLED) {
        // If CPU is halted, do not handle interrupt but just let it continue executing instructions again
        return instrContext.cpuState == CPUState::HALTED;
    }
    for (u8 shift = 0 ; shift <= 4 ; shift++) {
        u8 bitMask = 1u << shift;
        if (!(_intr & bitMask)) {
            continue;
        }
        auto interruptType = static_cast<InterruptType>(shift);
        memory_address addr = getInterruptStartAddress(interruptType);
        instrContext.interruptMasterEnable = IMEState::DISABLED;
        // TODO: do 2 NOP cycles (when implementing cycle accuracy)
        instrContext.push16Bits(instrContext.progCounter);

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        FunkyBoy::Debug::writeInterruptToLog(addr, file);
#endif

        instrContext.progCounter = addr;
        debug_print_4("Do interrupt at 0x%04X\n", addr);
        _if &= ~bitMask;
        memory->write8BitsTo(FB_REG_IF, _if); // TODO: Investigate "Timer doesn't work" error from ROM output
        // TODO: Interrupt Service Routine should take 5 cycles
        return true;
    }
    return false;
}

void CPU::doTimers() {
    ioRegisters->incrementSysCounter();
    if (timerOverflowingCycles != -1 && --timerOverflowingCycles == 0) {
        //fprintf(stdout, "# Request Timer interrupt\n");
        memory->write8BitsTo(FB_REG_TIMA, memory->read8BitsAt(FB_REG_TMA));
        requestInterrupt(InterruptType::TIMER);
        timerOverflowingCycles = -1;
    }
    u8 tac = memory->read8BitsAt(FB_REG_TAC);
    bool comp1 = (tac & 0b100u) != 0;
    switch (tac & 0b11u) {
        case 0u:
            comp1 &= (ioRegisters->sysCounterMsb() & 0b10) != 0;
            break;
        case 1u:
            comp1 &= (ioRegisters->sysCounterLsb() & 0b1000) != 0;
            break;
        case 2u:
            comp1 &= (ioRegisters->sysCounterLsb() & 0b100000) != 0;
            break;
        case 3u:
            comp1 &= (ioRegisters->sysCounterLsb() & 0b10000000) != 0;
            break;
        default:
            fprintf(stderr, "Illegal timer frequency\n");
            return;
    }
    // Falling edge detector
    if (delayedTIMAIncrease && !comp1) {
        u8 tima = memory->read8BitsAt(FB_REG_TIMA);
        if (tima == 0xff) {
            //fprintf(stdout, "# TIMA has overflown\n");
            // Delay TIMA load by 1 m-cycle
            timerOverflowingCycles = 4;
            // In the meantime, set TIMA to 0
            memory->write8BitsTo(FB_REG_TIMA, 0x00);
        } else if (timerOverflowingCycles == -1) { // TIME has to be 0 for one full m-cycle, so we do not increase here in that case
            memory->write8BitsTo(FB_REG_TIMA, tima + 1);
        }
    }
    delayedTIMAIncrease = comp1;
}

void CPU::requestInterrupt(InterruptType type) {
    //fprintf(stdout, "#req int %d\n", type);
    u8 _if = memory->read8BitsAt(FB_REG_IF);
    memory->write8BitsTo(FB_REG_IF, _if | getInterruptBitMask(type));
}

void CPU::setProgramCounter(u16 offset) {
    instrContext.progCounter = offset;
}

u16 CPU::readAF() {
    return (*regF_do_not_use_directly & 0b11110000) | (*regA << 8);
}

void CPU::writeAF(FunkyBoy::u16 val) {
    *regF_do_not_use_directly = val & 0b11110000; // Only the 4 most significant bits are written to register F
    *regA = (val >> 8) & 0xff;
}