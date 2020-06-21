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

#ifndef FB_CORE_INSTRUCTIONS_ALU_H
#define FB_CORE_INSTRUCTIONS_ALU_H

#include <instructions/instruction_context.h>

namespace FunkyBoy::Instructions {

    /**
     * ADD A,r
     * @param context
     */
    void add_A_r(InstrContext &context);

    /**
     * ADD A,d
     * @param context
     */
    void add_A_d(InstrContext &context);

    /**
     * ADC A,r
     * @param context
     */
    void adc_A_r(InstrContext &context);

    /**
     * ADC A,d
     * @param context
     */
    void adc_A_d(InstrContext &context);

    /**
     * ADD HL,ss
     * @param context
     */
    void add_HL_ss(InstrContext &context);

    /**
     * ADD HL,SP
     * @param context
     */
    void add_HL_SP(InstrContext &context);

    /**
     * ADD SP,e
     * @param context
     */
    void add_SP_e(InstrContext &context);

    /**
     * ADD A,(HL)
     * @param context
     */
    void add_A_HL(InstrContext &context);

    /**
     * ADC A,(HL)
     * @param context
     */
    void adc_A_HL(InstrContext &context);

    /**
     * SUB A,r
     * @param context
     */
    void sub_A_r(InstrContext &context);

    /**
     * SUB A,d
     * @param context
     */
    void sub_A_d(InstrContext &context);

    /**
     * SBC A,r
     * @param context
     */
    void sbc_A_r(InstrContext &context);

    /**
     * SBC A,d
     * @param context
     */
    void sbc_A_d(InstrContext &context);

    /**
     * SUB (HL)
     * @param context
     */
    void sub_HL(InstrContext &context);

    /**
     * SBC A,(HL)
     * @param context
     */
    void sbc_A_HL(InstrContext &context);

    /**
     * CP r
     * @param context
     */
    void cp_r(InstrContext &context);

    /**
     * CP d
     * @param context
     */
    void cp_d(InstrContext &context);

    /**
     * CP (HL)
     * @param context
     */
    void cp_HL(InstrContext &context);

    /**
     * INC ss
     * @param context
     */
    void inc_ss(InstrContext &context);

    /**
     * INC SP
     * @param context
     */
    void inc_SP(InstrContext &context);

    /**
     * INC (HL)
     * @param context
     */
    void inc_HL(InstrContext &context);

    /**
     * INC r
     * @param context
     */
    void inc_r(InstrContext &context);

    /**
     * DEC ss
     * @param context
     */
    void dec_ss(InstrContext &context);

    /**
     * DEC SP
     * @param context
     */
    void dec_SP(InstrContext &context);

    /**
     * DEC HL
     * @param context
     */
    void dec_HL(InstrContext &context);

    /**
     * DEC r
     * @param context
     */
    void dec_r(InstrContext &context);

    /**
     * OR r
     * @param context
     */
    void or_r(InstrContext &context);

    /**
     * OR d
     * @param context
     */
    void or_d(InstrContext &context);

    /**
     * OR (HL)
     * @param context
     */
    void or_HL(InstrContext &context);

    /**
     * AND r
     * @param context
     */
    void and_r(InstrContext &context);

    /**
     * AND d
     * @param context
     */
    void and_d(InstrContext &context);

    /**
     * AND (HL)
     * @param context
     */
    void and_HL(InstrContext &context);

    /**
     * XOR r
     * @param context
     */
    void xor_r(InstrContext &context);

    /**
     * XOR d
     * @param context
     */
    void xor_d(InstrContext &context);

    /**
     * XOR (HL)
     * @param context
     */
    void xor_HL(InstrContext &context);

}

#endif //FB_CORE_INSTRUCTIONS_ALU_H
