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

#include <util/typedefs.h>
#include <util/registers.h>
#include <util/return_codes.h>
#include <emulator/io_registers.h>

using namespace FunkyBoy;

CPU::CPU(GameBoyType gbType, const io_registers& ioRegisters)
    : gbType(gbType)
    , ioRegisters(ioRegisters)
    , instrContext(gbType)
    , timerOverflowingCycles(-1)
    , delayedTIMAIncrease(false)
    , operandIndex(0)
    , joypadWasNotPressed(true)
#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    , file("exec_opcodes_fb_v2.txt")
    , instr(0)
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
    instrContext.operands = operands;
    instrContext.interruptMasterEnable = IMEState::DISABLED;
    instrContext.haltBugRequested = false;
    instrContext.cpuState = CPUState::RUNNING;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    instrContext.executionLog = &file;
#endif

    // Fetch/Execute overlapping -> initial fetch is performed without executing any other instruction
    // To simulate this, we set a NOP as the first instruction, which does nothing
    operands[0] = Operands::nop;
    operands[1] = nullptr;
}

void CPU::powerUpInit(Memory &memory, Controller::Controllers &controllers) {
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

    memory.write8BitsTo(0xff05, 0x00);
    memory.write8BitsTo(0xff06, 0x00);
    memory.write8BitsTo(0xff07, 0x00);
    memory.write8BitsTo(0xff10, 0x80);
    memory.write8BitsTo(0xff11, 0xbf);
    memory.write8BitsTo(0xff12, 0xf3);
    memory.write8BitsTo(0xff14, 0xbf);
    memory.write8BitsTo(0xff16, 0x3f);
    memory.write8BitsTo(0xff17, 0x00);
    memory.write8BitsTo(0xff19, 0xbf);
    memory.write8BitsTo(0xff1a, 0x7f);
    memory.write8BitsTo(0xff1b, 0xff);
    memory.write8BitsTo(0xff1c, 0x9f);
    memory.write8BitsTo(0xff1e, 0xbf);
    memory.write8BitsTo(0xff20, 0xff);
    memory.write8BitsTo(0xff21, 0x00);
    memory.write8BitsTo(0xff22, 0x00);
    memory.write8BitsTo(0xff23, 0xbf);
    memory.write8BitsTo(0xff24, 0x77);
    memory.write8BitsTo(0xff25, 0xf3);
    if (gbType == GameBoyDMG) {
        memory.write8BitsTo(0xff26, 0xf1);
    } else {
        // TODO: This is for SGB, does this also apply for CGB?
        memory.write8BitsTo(0xff26, 0xf0);
    }
    memory.write8BitsTo(0xff40, 0x91);
    memory.write8BitsTo(0xff42, 0x00);
    memory.write8BitsTo(0xff43, 0x00);
    memory.write8BitsTo(0xff45, 0x00);
    memory.write8BitsTo(0xff47, 0xfc);
    memory.write8BitsTo(0xff48, 0xff);
    memory.write8BitsTo(0xff49, 0xff);
    memory.write8BitsTo(0xff4a, 0x00);
    memory.write8BitsTo(0xff4b, 0x00);
    memory.write8BitsTo(0xffff, 0x00);

    // Initialize Joypad
    ioRegisters.updateJoypad(controllers);
}

ret_code CPU::doMachineCycle(Memory &memory, Controller::Controllers &controllers) {
    doJoypad(controllers);
    auto result = doCycle(memory);

    // TODO: Handle timer here or earlier?
    doTimers(memory, 4);

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

    return result;
}

ret_code CPU::doCycle(Memory &memory) {
    if (instrContext.cpuState == CPUState::STOPPED) {
        return FB_RET_SUCCESS | FB_RET_SKIPPED;
    }

    bool shouldDoInterrupts = instrContext.cpuState == CPUState::HALTED;
    bool shouldFetch = false;
    bool interruptServiced = false;

    ret_code result = FB_RET_SUCCESS;

    if (instrContext.cpuState == CPUState::RUNNING) {
        memory.doDMA(); // TODO: Implement delay of 2 clocks

        auto op = operands[operandIndex++];

        if (operands[operandIndex] == nullptr) {
            shouldFetch = true;
            result |= FB_RET_INSTRUCTION_DONE;
        }

        if (!op(instrContext, memory)) {
            shouldFetch = true;
        }
        shouldDoInterrupts = shouldFetch;
    }

#if defined(FB_TESTING)
    if (shouldFetch || shouldDoInterrupts) {
        instructionCompleted = true;
    }
#endif

    if (shouldDoInterrupts && (instrContext.cpuState == CPUState::RUNNING || instrContext.cpuState == CPUState::HALTED) && doInterrupts(memory)) {
        // An interrupt has started (or IME=false && (IF & IE) != 0), so we set the state to RUNNING (again)
        instrContext.cpuState = CPUState::RUNNING;
        interruptServiced = true;
    }

    if (interruptServiced || (shouldFetch && instrContext.cpuState == CPUState::RUNNING)) { // TODO: Can this be simplified to just instrContext.cpuState == CPUState::RUNNING ?
        result |= doFetchAndDecode(memory);
        operandIndex = 0;
        return result;
    }

    return result;
}

ret_code CPU::doFetchAndDecode(Memory &memory) {
    if (!instrContext.haltBugRequested) {
        instrContext.instr = memory.read8BitsAt(instrContext.progCounter++);
    } else {
        instrContext.instr = memory.read8BitsAt(instrContext.progCounter);
        instrContext.haltBugRequested = false;
    }

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    FunkyBoy::Debug::writeExecutionToLog('I', file, instrContext);
    instr++;
#endif

    if (!Operands::decodeOpcode(instrContext.instr, operands)) {
        fprintf(stderr, "Illegal instruction 0x%02X at 0x%04X\n", instrContext.instr, instrContext.progCounter - 1);
        return 0;
    }
    return FB_RET_SUCCESS;
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

void CPU::doJoypad(Controller::Controllers &controllers) {
    u8 oldP1 = ioRegisters.getP1() & 0b00001111u;
    u8 newP1 = ioRegisters.updateJoypad(controllers) & 0b00001111u;
    bool isNotPressed = oldP1 & newP1;
    if (!isNotPressed && joypadWasNotPressed) {
        requestInterrupt(InterruptType::JOYPAD);
    }
    if (!isNotPressed && instrContext.cpuState == CPUState::STOPPED) {
        instrContext.cpuState = CPUState::RUNNING;
    }
    joypadWasNotPressed = isNotPressed;
}

bool CPU::doInterrupts(Memory &memory) {
    if (instrContext.cpuState == CPUState::STOPPED) {
        return false;
    }
    u8 &_if = ioRegisters.getIF();
    _if %= 0x1fu;
    u8 _ie = memory.read8BitsAt(FB_REG_IE) & 0x1fu;
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
        instrContext.push16Bits(memory, instrContext.progCounter);

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        FunkyBoy::Debug::writeInterruptToLog(addr, file);
#endif

        instrContext.progCounter = addr;
        debug_print_4("Do interrupt at 0x%04X\n", addr);
        _if &= ~bitMask; // Writes directly to io_registry
        // TODO: Interrupt Service Routine should take 5 cycles
        return true;
    }
    return false;
}

bool doTimerObscureCheck(u8 clocks, u16 sysCounter, u8 tac) {
    while (clocks > 0) {
        switch (tac & 0b11u) {
            case 0u: {
                if (sysCounter & 0b1000000000u) {
                    return true;
                }
                break;
            }
            case 1u: {
                if (sysCounter & 0b1000u) {
                    return true;
                }
                break;
            }
            case 2u: {
                if (sysCounter & 0b100000u) {
                    return true;
                }
                break;
            }
            case 3u: {
                if (sysCounter & 0b10000000u) {
                    return true;
                }
                break;
            }
            default: {
                fprintf(stderr, "Illegal timer frequency\n");
                return false;
            }
        }
        sysCounter++;
        clocks--;
    }
    return false;
}

void CPU::doTimers(Memory &memory, u8 clocks) {
    u16 sysCounter = ioRegisters.getSysCounter();
    ioRegisters.setSysCounter(sysCounter + clocks); // TODO: Move to end of this function?
    if (timerOverflowingCycles != -1) {
        timerOverflowingCycles -= clocks;
        if (timerOverflowingCycles <= 0) {
            //fprintf(stdout, "# Request Timer interrupt\n");
            memory.write8BitsTo(FB_REG_TIMA, memory.read8BitsAt(FB_REG_TMA));
            requestInterrupt(InterruptType::TIMER);
            timerOverflowingCycles = -1;
        }
    }
    u8 tac = memory.read8BitsAt(FB_REG_TAC);
    bool comp1 = (tac & 0b100u) != 0;
    comp1 &= doTimerObscureCheck(clocks, sysCounter, tac);
    // Falling edge detector
    if (delayedTIMAIncrease && !comp1) {
        u8 tima = memory.read8BitsAt(FB_REG_TIMA);
        if (tima == 0xff) {
            //fprintf(stdout, "# TIMA has overflown\n");
            // Delay TIMA load by 1 m-cycle
            timerOverflowingCycles = 4;
            // In the meantime, set TIMA to 0
            memory.write8BitsTo(FB_REG_TIMA, 0x00);
        } else if (timerOverflowingCycles == -1) { // TIME has to be 0 for one full m-cycle, so we do not increase here in that case
            memory.write8BitsTo(FB_REG_TIMA, tima + 1);
        }
    }
    delayedTIMAIncrease = comp1;
}

void CPU::requestInterrupt(InterruptType type) {
    //fprintf(stdout, "#req int %d\n", type);
    u8 &_if = ioRegisters.getIF();
    _if |= getInterruptBitMask(type);
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