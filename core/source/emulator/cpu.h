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
#include <instructions/instructions.h>
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
        io_registers ioRegisters;
        MemoryPtr memory;
        const GameBoyType gbType;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        unsigned long long instr;
        std::ofstream file;
#endif

        register_t registers[8]{};

        i8 timerOverflowingCycles;
        bool delayedTIMAIncrease;

        bool joypadWasNotPressed;

        void powerUpInit();

        ret_code doCycle();

        void doJoypad();
        bool doInterrupts();
        void doTimers(u8 clocks);

    test_public:

        Instructions::context instrContext;
        opcode_t currentOpcode;

        u16 readAF();
        void writeAF(u16 val);

#if defined(FB_TESTING)
        bool instructionCompleted;
#endif

        // Do not free these pointers, they are proxies to specific locations in the registers array

        register_t *regB;
        register_t *regC;
        register_t *regD;
        register_t *regE;
        register_t *regH;
        register_t *regL;
        register_t *regF_do_not_use_directly;
        register_t *regA;

    public:
        CPU(GameBoyType gbType, MemoryPtr memory, const io_registers& ioRegisters);

        void setProgramCounter(u16 offset);
        void requestInterrupt(InterruptType type);

        ret_code doMachineCycle();
    };

    typedef std::shared_ptr<CPU> CPUPtr;

}

#endif //FB_CORE_CPU_H
