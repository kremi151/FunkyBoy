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

#ifndef FB_CORE_INSTRUCTIONS_ROT_SHIFTS_H
#define FB_CORE_INSTRUCTIONS_ROT_SHIFTS_H

#include <instructions/instruction_context.h>

namespace FunkyBoy::Instructions {

    /**
     * RRCA
     * @param context
     * @return
     */
    bool rrca(InstrContext &context);

    /**
     * RLCA
     * @param context
     * @return
     */
    bool rlca(InstrContext &context);

    /**
     * RRA
     * @param context
     * @return
     */
    bool rra(InstrContext &context);

    /**
     * RLA
     * @param context
     * @return
     */
    bool rla(InstrContext &context);

}

#endif //FB_CORE_INSTRUCTIONS_ROT_SHIFTS_H
