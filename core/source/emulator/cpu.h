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

namespace FunkyBoy {

    class CPU {
    private:
        std::shared_ptr<Memory> memory;

        u8 registers[8]{};
        u16 progCounter;
        u16 stackPointer;

        // Do not free these pointers, they are proxies to specific locations in the registers array

        u8 *regB;
        u8 *regC;
        u8 *regD;
        u8 *regE;
        u8 *regH;
        u8 *regL;
        u8 *regF;
        u8 *regA;

        inline bool isCarry();
        void setCarry(bool carry);

        inline bool isHalfCarry();
        void setHalfCarry(bool halfCarry);

        inline bool isSubstraction();
        void setSubstraction(bool substration);

        inline bool isZero();
        void setZero(bool zero);

        void setFlags(bool zero, bool subtraction, bool halfCarry, bool carry);

        void push16Bits(u16 val);
        u16 pop16Bits();

        void cp(u8 val);
        void _xor(u8 val);
        void adc(u8 val, bool carry);

        u16 readHL();
        void writeHL(u16 val);

        u16 readAF();

        u16 read16BitRegister(u8 position);
        void write16BitRegister(u8 position, u16 val);

        bool doPrefix(u8 prefix);
    public:
        explicit CPU(std::shared_ptr<Memory> memory);

        void setProgramCounter(u16 offset);

        bool doTick();
    };

}

#endif //CORE_CPU_H
