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

#ifndef CORE_CPU_H
#define CORE_CPU_H

#include "util/typedefs.h"

#include <memory/memory.h>
#include <memory>
#include <util/testing.h>
#include <util/debug.h>
#include <instructions/instructions.h>

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
#include <fstream>
#endif

namespace FunkyBoy {

    enum GameBoyType {
        GameBoyDMG,
        GameBoySGB,
        GameBoyCGB
    };

    enum IMEState {
        DISABLED = 0,
        REQUEST_ENABLE = 1,
        ENABLING = 2,
        ENABLED = 3
    };

    enum CPUState {
        RUNNING = 0,
        HALTED = 1,
        STOPPED = 2
    };

    enum InterruptType {
        VBLANK      = 0,
        LCD_STAT    = 1,
        TIMER       = 2,
        SERIAL      = 3,
        JOYPAD      = 4
    };

    class CPU {
    private:
        std::shared_ptr<Memory> memory;

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        unsigned long long instr;
        std::ofstream file;
#endif

        u8 registers[8]{};

        IMEState interruptMasterEnable;
        CPUState cpuState;
        InstrContext instrContext{};

        Operand operands[25]{};

        u16 timerCounter;
        u16 divCounter;

        void powerUpInit();

        inline GameBoyType getType();

        void doFetch();
        bool doDecode();
        bool __TODO_REWRITE__doPrefix(u8 prefix);

        bool doInterrupts();
        void doTimer();
        void doDivider();
        void requestInterrupt(InterruptType type);

    test_public:

        u16 readAF();
        void writeAF(u16 val);

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
        explicit CPU(std::shared_ptr<Memory> memory);

        void setProgramCounter(u16 offset);

        bool doTick();
    };

}

#endif //CORE_CPU_H
