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

#ifndef FB_CORE_INSTRUCTIONS_CONTEXT_H
#define FB_CORE_INSTRUCTIONS_CONTEXT_H

#include <util/typedefs.h>
#include <memory/memory.h>
#include <emulator/gb_type.h>
#include <emulator/state_types.h>

namespace FunkyBoy::Instructions {

    class context {
    public:
        explicit context(GameBoyType gbType);

        register_t *registers;
        register_t *regB;
        register_t *regC;
        register_t *regD;
        register_t *regE;
        register_t *regH;
        register_t *regL;
        register_t *regF;
        register_t *regA;

        u16 progCounter;
        u16 stackPointer;

        CPUState cpuState;
        IMEState interruptMasterEnable;

        bool haltBugRequested;

        const GameBoyType gbType;

        inline u16_fast readHL() {
            return (*regL & 0xffu) | (*regH << 8u);
        }

        void writeHL(u16_fast val);

        void push16Bits(FunkyBoy::Memory &memory, u16_fast val);
        u16_fast pop16Bits(FunkyBoy::Memory &memory);

        void write16BitRegister(u8_fast position, u16_fast val);
        u16_fast read16BitRegister(u8_fast position);
    };

}

#endif //FB_CORE_INSTRUCTIONS_CONTEXT_H
