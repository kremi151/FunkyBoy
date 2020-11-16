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

#ifndef FB_CORE_OPERANDS_WRITES_H
#define FB_CORE_OPERANDS_WRITES_H

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands {

    /**
     * Writes 16 bits composed by {@code lsb} and {@code msb} into 16bit register.
     * The 16bit register is determined by reading bits 4 and 5 of the current {@code instr}.
     * @param context
     * @return
     */
    bool write16BitsIntoRR(InstrContext &context, Memory &memory);

    /**
     * Writes 16 bits composed by {@code lsb} and {@code msb} into AF register
     * @param context
     * @return
     */
    bool write16BitsIntoAF(InstrContext &context, Memory &memory);

    /**
     * Writes {@code lsb} into (HL)
     * @param context
     * @return
     */
    bool writeLSBIntoHLMem(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_OPERANDS_WRITES_H
