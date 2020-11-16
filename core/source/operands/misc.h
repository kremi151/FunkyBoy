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

#ifndef FB_CORE_OPERANDS_MISC_H
#define FB_CORE_OPERANDS_MISC_H

#include "instruction_context.h"

namespace FunkyBoy::Operands {

    /**
     * Does nothing, causes nothing, changes nothing.
     * @param context
     * @return
     */
    bool nop(InstrContext &context, Memory &memory);

    /**
     * Like {@code nop}, but used as a marker for padding instructions to reach a certain amount of cycles
     * @param context
     * @return
     */
    bool _pad_(InstrContext &context, Memory &memory);

    /**
     * Enables interrupts without delay
     * @param context
     * @return
     */
    bool enableInterruptsImmediately(InstrContext &context, Memory &memory);

    /**
     * Enables interrupts with delay.
     * Interrupts will become enabled after the next opcode.
     * @param context
     * @return
     */
    bool enableInterruptsDelayed(InstrContext &context, Memory &memory);

    /**
     * Disables interrupts
     * @param context
     * @return
     */
    bool disableInterrupts(InstrContext &context, Memory &memory);

    /**
     * STOP
     * @param context
     * @return
     */
    bool stop(InstrContext &context, Memory &memory);

    /**
     * HALT
     * @param context
     * @return
     */
    bool halt(InstrContext &context, Memory &memory);

    /**
     * DAA
     * @param context
     * @return
     */
    bool daa(InstrContext &context, Memory &memory);

    /**
     * CPL
     * @param context
     * @return
     */
    bool cpl(InstrContext &context, Memory &memory);

    /**
     * SCF
     * @param context
     * @return
     */
    bool scf(InstrContext &context, Memory &memory);

    /**
     * CCF
     * @param context
     * @return
     */
    bool ccf(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_OPERANDS_MISC_H
