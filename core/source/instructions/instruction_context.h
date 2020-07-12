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

#ifndef FB_CORE_INSTRUCTION_CONTEXT_H
#define FB_CORE_INSTRUCTION_CONTEXT_H

#include <util/typedefs.h>
#include <memory/memory.h>
#include <emulator/gb_type.h>
#include <instructions/debug.h>

namespace FunkyBoy {

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

    class InstrContext;

    typedef bool (*Operand)(InstrContext &context);

    class InstrContext {
    public:
        InstrContext(GameBoyType gbType);

        const GameBoyType gbType;

        u8 instr;
        u8 *registers;
        u8 *regB;
        u8 *regC;
        u8 *regD;
        u8 *regE;
        u8 *regH;
        u8 *regL;
        u8 *regF;
        u8 *regA;

        MemoryPtr memory;

        u8 lsb;
        u8 msb;
        i8 signedByte;

        u16 progCounter;
        u16 stackPointer;

        Operand *operands;

        CPUState cpuState;
        IMEState interruptMasterEnable;

        bool haltBugRequested;

        /* inline */ u16 readHL() {
            return (*regL & 0xffu) | (*regH << 8u);
        }

        void writeHL(u16 val);

        void push16Bits(u16 val);
        void push16Bits(u8 msb, u8 lsb);
        u16 pop16Bits();

        void write16BitRegister(u8 position, u16 val);
        u16 read16BitRegister(u8 position);

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
        std::ofstream *executionLog;
#endif
    };

}

#endif //FB_CORE_INSTRUCTION_CONTEXT_H
