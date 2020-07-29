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

#ifndef FB_CORE_CPU_H
#define FB_CORE_CPU_H

#include "util/typedefs.h"

#include <memory/memory.h>
#include <memory>
#include <util/testing.h>
#include <util/debug.h>
#include <instructions/decoder.h>
#include <instructions/debug.h>
#include <emulator/gb_type.h>
#include <emulator/io_registers.h>

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
#include <fstream>
#endif

namespace FunkyBoy {

    enum InterruptType {
        VBLANK      = 0,
        LCD_STAT    = 1,
        TIMER       = 2,
        SERIAL      = 3,
        JOYPAD      = 4
    };

    class CPU {
    private:
        io_registers_ptr ioRegisters;
        MemoryPtr memory;
        const GameBoyType gbType;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        unsigned long long instr;
        std::ofstream file;
#endif

        u8 registers[8]{};

        u8 operandIndex;
        Operand operands[25]{};

        u8 cpuInterCycleCounter;

        i8 timerOverflowingCycles;
        bool delayedTIMAIncrease;

        bool joypadWasNotPressed;

        void powerUpInit();

        bool doCycle();
        bool doFetchAndDecode();

        void doJoypad();
        bool doInterrupts();
        void doTimers();
        void requestInterrupt(InterruptType type);

    test_public:

        InstrContext instrContext;

        u16 readAF();
        void writeAF(u16 val);

#if defined(FB_TESTING)
        bool instructionCompleted;
#endif

        // Do not free these pointers, they are proxies to specific locations in the registers array

        u8 *regB;
        u8 *regC;
        u8 *regD;
        u8 *regE;
        u8 *regH;
        u8 *regL;
        u8 *regF_do_not_use_directly;
        u8 *regA;

    public:
        CPU(GameBoyType gbType, MemoryPtr memory, io_registers_ptr ioRegisters);

        void setProgramCounter(u16 offset);

        bool doTick();
    };

}

#endif //FB_CORE_CPU_H
