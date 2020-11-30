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

#ifndef FB_CORE_INSTRUCTIONS_LOADS_H
#define FB_CORE_INSTRUCTIONS_LOADS_H

#include <instructions/context.h>
#include <memory/memory.h>

namespace FunkyBoy::Instructions::Loads {

    /**
     * LD r,r
     * @param context
     */
    void ld_reg_reg(opcode_t opcode, Instructions::context &context);

    /**
     * LD (a16),A
     */
    void ld_a16_A(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,(a16)
     */
    void ld_A_a16(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (C),A
     */
    void ld_C_A(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,(C)
     */
    void ld_A_C(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,d8
     */
    void ld_A_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (ss),d16
     */
    void ld_ss_d16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD SP,d16
     */
    void ld_SP_d16(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (a16),SP
     */
    void ld_a16_SP(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD r,d8
     */
    void ld_r_d8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (HL),d8
     */
    void ld_HL_d8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (ss),A
     */
    void ld_ss_A(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,(ss)
     */
    void ld_A_ss(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (HL+),A
     */
    void ld_HLI_A(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (HL-),A
     */
    void ld_HLD_A(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,(HL+)
     */
    void ld_A_HLI(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD A,(HL-)
     */
    void ld_A_HLD(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD (HL),r
     */
    void ld_HL_r(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD r,(HL)
     */
    void ld_r_HL(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD SP,HL
     * @param memory
     * @param context
     */
    void ld_SP_HL(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LD HL,SP+e8
     * @param memory
     * @param context
     */
    void ld_HL_SP_plus_e8(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LDH (a8),A
     * @param memory
     * @param context
     */
    void ldh_a8_A(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * LDH A,(a8)
     * @param memory
     * @param context
     */
    void ldh_A_a8(FunkyBoy::Memory &memory, Instructions::context &context);

}

#endif //FB_CORE_INSTRUCTIONS_LOADS_H
