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

namespace FunkyBoy::Instructions {

    typedef struct {
        u8_fast *registers;
        u8_fast *regB;
        u8_fast *regC;
        u8_fast *regD;
        u8_fast *regE;
        u8_fast *regH;
        u8_fast *regL;
        u8_fast *regF;
        u8_fast *regA;

        u16 progCounter;
        u16 stackPointer;
    } context;

}

#endif //FB_CORE_INSTRUCTIONS_CONTEXT_H
