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
#include <iostream>
#include <util/testing.h>
#include <util/debug.h>
#include <operands/instruction_context.h>
#include <operands/debug.h>
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
        const GameBoyType gbType;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        unsigned long long instr;
        std::ofstream file;
#endif

        i8 timerOverflowingCycles;
        bool delayedTIMAIncrease;

        bool joypadWasNotPressed;

        ret_code doCycle(Memory &memory);
        ret_code doFetchAndDecode(Memory &memory);

        void doJoypad();
        bool doInterrupts(Memory &memory);
        void doTimers(Memory &memory, u8 clocks);

        static bool doTimerObscureCheck(u8_fast clocks, u16_fast sysCounter, u8_fast tac);

    test_public:

        InstrContext instrContext;

        u16 readAF();
        void writeAF(u16 val);

#if defined(FB_TESTING)
        bool instructionCompleted;
#endif

        const Operand *operands; // Do not free this pointer

    public:
        CPU(GameBoyType gbType, const io_registers& ioRegisters);

        void powerUpInit(Memory &memory);

        void setProgramCounter(u16 offset);
        void requestInterrupt(InterruptType type);

        ret_code doMachineCycle(Memory &memory);

        void serialize(std::ostream &ostream) const;
        void deserialize(std::istream &istream);
    };

}

#endif //FB_CORE_CPU_H
