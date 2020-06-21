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

}

#endif //FB_CORE_INSTRUCTIONS_ALU_H
