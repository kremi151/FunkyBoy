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

#ifndef FB_CORE_OPERANDS_CONDITIONS_H
#define FB_CORE_OPERANDS_CONDITIONS_H

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands {

    /**
     * Checks whether zero flag is not set if opcode is in left half of the table (0xX0 - 0xX7), otherwise whether it is set.
     * If not set, the next opcode will be fetched in the next machine cycle, skipping remaining operands.
     * @param context
     * @return
     */
    bool checkIsZeroContextual(InstrContext &context, Memory &memory);

    /**
     * Checks whether carry flag is not set if opcode is in left half of the table (0xX0 - 0xX7), otherwise whether it is set.
     * If not set, the next opcode will be fetched in the next machine cycle, skipping remaining operands.
     * @param context
     * @return
     */
    bool checkIsCarryContextual(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_OPERANDS_CONDITIONS_H
