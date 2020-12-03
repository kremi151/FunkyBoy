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

#ifndef FB_CORE_OPERANDS_ALU_H
#define FB_CORE_OPERANDS_ALU_H

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands {

    /**
     * SBC A,(HL)
     * @param context
     */
    bool sbc_A_HL(InstrContext &context, Memory &memory);

    /**
     * CP r
     * @param context
     */
    bool cp_r(InstrContext &context, Memory &memory);

    /**
     * CP d
     * @param context
     */
    bool cp_d(InstrContext &context, Memory &memory);

    /**
     * CP (HL)
     * @param context
     */
    bool cp_HL(InstrContext &context, Memory &memory);

    /**
     * INC ss
     * @param context
     */
    bool inc_ss(InstrContext &context, Memory &memory);

    /**
     * INC SP
     * @param context
     */
    bool inc_SP(InstrContext &context, Memory &memory);

    /**
     * INC (HL)
     * @param context
     */
    bool inc_HL(InstrContext &context, Memory &memory);

    /**
     * INC r
     * @param context
     */
    bool inc_r(InstrContext &context, Memory &memory);

    /**
     * DEC ss
     * @param context
     */
    bool dec_ss(InstrContext &context, Memory &memory);

    /**
     * DEC SP
     * @param context
     */
    bool dec_SP(InstrContext &context, Memory &memory);

    /**
     * DEC HL
     * @param context
     */
    bool dec_HL(InstrContext &context, Memory &memory);

    /**
     * DEC r
     * @param context
     */
    bool dec_r(InstrContext &context, Memory &memory);

    /**
     * OR r
     * @param context
     */
    bool or_r(InstrContext &context, Memory &memory);

    /**
     * OR d
     * @param context
     */
    bool or_d(InstrContext &context, Memory &memory);

    /**
     * OR (HL)
     * @param context
     */
    bool or_HL(InstrContext &context, Memory &memory);

    /**
     * AND r
     * @param context
     */
    bool and_r(InstrContext &context, Memory &memory);

    /**
     * AND d
     * @param context
     */
    bool and_d(InstrContext &context, Memory &memory);

    /**
     * AND (HL)
     * @param context
     */
    bool and_HL(InstrContext &context, Memory &memory);

    /**
     * XOR r
     * @param context
     */
    bool xor_r(InstrContext &context, Memory &memory);

    /**
     * XOR d
     * @param context
     */
    bool xor_d(InstrContext &context, Memory &memory);

    /**
     * XOR (HL)
     * @param context
     */
    bool xor_HL(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_OPERANDS_ALU_H
