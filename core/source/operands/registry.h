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

#ifndef FB_CORE_OPERANDS_REGISTRY_H
#define FB_CORE_OPERANDS_REGISTRY_H

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands::Registry {
    extern const Operand nop[2];
    extern const Operand ld_r_r[2];
    extern const Operand ld_a16_A[5];
    extern const Operand ld_A_a16[5];
    extern const Operand ld_C_A[3];
    extern const Operand ld_A_C[3];
    extern const Operand ld_A_d8[3];
    extern const Operand ld_ss_d16[4];
    extern const Operand ld_SP_d16[4];
    extern const Operand ld_a16_SP[6];
    extern const Operand ld_r_d8[3];
    extern const Operand ld_HL_d8[4];
    extern const Operand ld_ss_A[3];
    extern const Operand ld_A_ss[3];
    extern const Operand ld_HLI_A[3];
    extern const Operand ld_HLD_A[3];
    extern const Operand ld_A_HLI[3];
    extern const Operand ld_A_HLD[3];
    extern const Operand ld_HL_r[3];
    extern const Operand ld_r_HL[3];
    extern const Operand ld_SP_HL[3];
    extern const Operand ld_HL_SP_e8[4];
    extern const Operand ldh_a8_A[4];
    extern const Operand ldh_A_a8[4];

    extern const Operand add_A_r[2];
    extern const Operand adc_A_r[2];
    extern const Operand add_A_d8[3];
    extern const Operand adc_A_d8[3];
    extern const Operand add_HL_ss[3];
    extern const Operand add_HL_SP[3];
    extern const Operand add_SP_r8[5];
    extern const Operand add_A_HL[3];
    extern const Operand adc_A_HL[3];

    extern const Operand sub_A_r[2];
    extern const Operand sbc_A_r[2];
    extern const Operand sub_A_d8[3];
    extern const Operand sbc_A_d8[3];
    extern const Operand sub_HL[3];
    extern const Operand sbc_HL[3];

    extern const Operand jp_N_Z_a16[5];
    extern const Operand jp_N_C_a16[5];
    extern const Operand jp[5];
    extern const Operand jp_HL[2];

    extern const Operand jr_N_Z_r8[4];
    extern const Operand jr_N_C_r8[4];
    extern const Operand jr[4];

    extern const Operand call_N_Z_a16[7];
    extern const Operand call_N_C_a16[7];
    extern const Operand call_a16[7];

    extern const Operand ret_N_Z[6];
    extern const Operand ret_N_C[6];
    extern const Operand ret[5];

    extern const Operand reti[5];

    extern const Operand rst_vec[5];

    extern const Operand cp_r[2];
    extern const Operand cp_HL[3];
    extern const Operand cp_d8[3];

    extern const Operand inc_ss[3];
    extern const Operand inc_SP[3];
    extern const Operand inc_HL[4];
    extern const Operand inc_r[2];

    extern const Operand dec_ss[3];
    extern const Operand dec_SP[3];
    extern const Operand dec_HL[4];
    extern const Operand dec_r[2];

    extern const Operand or_r[2];
    extern const Operand or_HL[3];
    extern const Operand or_d8[3];

    extern const Operand and_r[2];
    extern const Operand and_HL[3];
    extern const Operand and_d8[3];

    extern const Operand xor_r[2];
    extern const Operand xor_HL[3];
    extern const Operand xor_d8[3];

    extern const Operand rrca[2];

    extern const Operand rlca[2];

    extern const Operand rra[2];

    extern const Operand rla[2];

    extern const Operand pop_rr[4];
    extern const Operand pop_AF[4];

    extern const Operand push_rr[5];
    extern const Operand push_AF[5];

    extern const Operand daa[2];

    extern const Operand cpl[2];

    extern const Operand scf[2];

    extern const Operand ccf[2];

    extern const Operand di[2];

    extern const Operand ei[2];

    extern const Operand stop[2];

    extern const Operand halt[2];
}

#endif //FB_CORE_OPERANDS_REGISTRY_H
