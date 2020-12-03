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

#include <instructions/context.h>
#include <memory/memory.h>

namespace FunkyBoy::Instructions::ALU {

    /**
     * ADD A,r
     */
    void add_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADC A,r
     */
    void adc_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADD A,d8
     */
    void add_A_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADC A,d8
     */
    void adc_A_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADD HL,ss
     */
    void add_HL_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADD HL,SP
     */
    void add_HL_SP(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADD SP,e8
     */
    void add_SP_e8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADD A,(HL)
     */
    void add_A_HL(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * ADC A,(HL)
     */
    void adc_A_HL(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SUB A,r
     */
    void sub_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SBC A,r
     */
    void sbc_A_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SUB A,d8
     */
    void sub_A_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SBC A,d8
     */
    void sbc_A_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SUB (HL)
     */
    void sub_HL(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * SBC A,(HL)
     */
    void sbc_A_HL(FunkyBoy::Memory &memory, Instructions::context &context);

}

#endif //FB_CORE_INSTRUCTIONS_ALU_H
