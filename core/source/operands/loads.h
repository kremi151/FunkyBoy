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

#ifndef FB_CORE_OPERANDS_LOADS_H
#define FB_CORE_OPERANDS_LOADS_H

#ifndef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include "instruction_context.h"

namespace FunkyBoy::Operands {

    /**
     * LD r,r
     * @param context
     */
    bool load_r_r(InstrContext &context, Memory &memory);

    /**
     * LD (dd),A
     * @param context
     */
    bool load_mem_dd_A(InstrContext &context, Memory &memory);

    /**
     * LD A,(dd)
     * @param context
     */
    bool load_A_mem_dd(InstrContext &context, Memory &memory);

    /**
     * LD A,d
     * @param context
     */
    bool load_A_d(InstrContext &context, Memory &memory);

    /**
     * LD dd,nn
     * @param context
     */
    bool load_dd_nn(InstrContext &context, Memory &memory);

    /**
     * LD SP,nn
     * @param context
     */
    bool load_SP_nn(InstrContext &context, Memory &memory);

    /**
     * LD nn,SP
     * @param context
     */
    bool load_nn_SP(InstrContext &context, Memory &memory);

    /**
     * LD r,n
     */
    bool load_r_n(InstrContext &context, Memory &memory);

    /**
     * LD (HL),n
     * @param context
     */
    bool load_HL_n(InstrContext &context, Memory &memory);

    /**
     * LD (dd),A
     * @param context
     */
    bool load_reg_dd_A(InstrContext &context, Memory &memory);

    /**
     * LD A,(dd)
     * @param context
     */
    bool load_A_reg_dd(InstrContext &context, Memory &memory);

    /**
     * LD (HL+),A
     * @param context
     */
    bool load_HLI_A(InstrContext &context, Memory &memory);

    /**
     * LD (HL-),A
     * @param context
     */
    bool load_HLD_A(InstrContext &context, Memory &memory);

    /**
     * LD A,(HL+)
     * @param context
     */
    bool load_A_HLI(InstrContext &context, Memory &memory);

    /**
     * LD A,(HL-)
     * @param context
     */
    bool load_A_HLD(InstrContext &context, Memory &memory);

    /**
     * LD (HL),r
     * @param context
     */
    bool load_HL_r(InstrContext &context, Memory &memory);

    /**
     * LD r,(HL)
     * @param context
     */
    bool load_r_HL(InstrContext &context, Memory &memory);

    /**
     * LD SP,HL
     * @param context
     */
    bool load_SP_HL(InstrContext &context, Memory &memory);

    /**
     * LD HL,SP+e
     * @param context
     */
    bool load_HL_SPe(InstrContext &context, Memory &memory);

}

#endif

#endif //FB_CORE_OPERANDS_LOADS_H
