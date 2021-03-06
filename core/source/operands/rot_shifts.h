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

#ifndef FB_CORE_OPERANDS_ROT_SHIFTS_H
#define FB_CORE_OPERANDS_ROT_SHIFTS_H

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands {

    /**
     * RRCA
     * @param context
     * @return
     */
    bool rrca(InstrContext &context, Memory &memory);

    /**
     * RLCA
     * @param context
     * @return
     */
    bool rlca(InstrContext &context, Memory &memory);

    /**
     * RRA
     * @param context
     * @return
     */
    bool rra(InstrContext &context, Memory &memory);

    /**
     * RLA
     * @param context
     * @return
     */
    bool rla(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_OPERANDS_ROT_SHIFTS_H
