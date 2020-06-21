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

namespace FunkyBoy {

    typedef struct {
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

        u16 progCounter;
        u16 stackPointer;
    } InstrContext;

    typedef void (*Operand)(InstrContext &context);

}

#endif //FB_CORE_INSTRUCTION_CONTEXT_H
