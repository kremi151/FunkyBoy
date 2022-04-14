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

#ifndef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include "registry.h"

#include <operands/instructions.h>

namespace FunkyBoy::Operands::Registry {
    const Operand nop[2] = {
            Operands::nop,
            nullptr
    };
    const Operand ld_r_r[2] = {
            Operands::load_r_r,
            nullptr
    };
    const Operand ld_a16_A[5] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_mem_dd_A,
            nullptr
    };
    const Operand ld_A_a16[5] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_A_mem_dd,
            nullptr
    };
    const Operand ld_C_A[3] = {
            Operands::readRegCAsLSB,
            Operands::load_mem_dd_A,
            nullptr
    };
    const Operand ld_A_C[3] = {
            Operands::readRegCAsLSB,
            Operands::load_A_mem_dd,
            nullptr
    };
    const Operand ld_A_d8[3] = {
            Operands::readLSB,
            Operands::load_A_d,
            nullptr
    };
    const Operand ld_ss_d16[4] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::load_dd_nn,
            nullptr
    };
    const Operand ld_SP_d16[4] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::load_SP_nn,
            nullptr
    };
    const Operand ld_a16_SP[6] = {
            Operands::readLSB,
            Operands::readMSB,
            // Pad artificially to 5 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::load_nn_SP,
            nullptr
    };
    const Operand ld_r_d8[3] = {
            Operands::readLSB,
            Operands::load_r_n,
            nullptr
    };
    const Operand ld_HL_d8[4] = {
            Operands::readLSB,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_HL_n,
            nullptr
    };
    const Operand ld_ss_A[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_reg_dd_A,
            nullptr
    };
    const Operand ld_A_ss[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_A_reg_dd,
            nullptr
    };
    const Operand ld_HLI_A[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_HLI_A,
            nullptr
    };
    const Operand ld_HLD_A[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_HLD_A,
            nullptr
    };
    const Operand ld_A_HLI[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_A_HLI,
            nullptr
    };
    const Operand ld_A_HLD[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_A_HLD,
            nullptr
    };
    const Operand ld_HL_r[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_HL_r,
            nullptr
    };
    const Operand ld_r_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_r_HL,
            nullptr
    };
    const Operand ld_SP_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::load_SP_HL,
            nullptr
    };
    const Operand ld_HL_SP_e8[4] = {
            Operands::readSigned,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_HL_SPe,
            nullptr
    };
    const Operand ldh_a8_A[4] = {
            Operands::readMemAsLSB,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_mem_dd_A,
            nullptr
    };
    const Operand ldh_A_a8[4] = {
            Operands::readMemAsLSB,
            Operands::_pad_, // TODO: do something useful here
            Operands::load_A_mem_dd,
            nullptr
    };

    const Operand add_A_r[2] = {
            Operands::add_A_r,
            nullptr
    };
    const Operand adc_A_r[2] = {
            Operands::adc_A_r,
            nullptr
    };
    const Operand add_A_d8[3] = {
            Operands::readLSB,
            Operands::add_A_d,
            nullptr
    };
    const Operand adc_A_d8[3] = {
            Operands::readLSB,
            Operands::adc_A_d,
            nullptr
    };
    const Operand add_HL_ss[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::add_HL_ss,
            nullptr
    };
    const Operand add_HL_SP[3] = {
            Operands::_pad_, // TODO: do something useful here,
            Operands::add_HL_SP,
            nullptr
    };
    const Operand add_SP_r8[5] = {
            Operands::readSigned,
            // Pad artificially to 4 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::add_SP_e,
            nullptr
    };
    const Operand add_A_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::add_A_HL,
            nullptr
    };
    const Operand adc_A_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::adc_A_HL,
            nullptr
    };

    const Operand sub_A_r[2] = {
            Operands::sub_A_r,
            nullptr
    };
    const Operand sbc_A_r[2] = {
            Operands::sbc_A_r,
            nullptr
    };
    const Operand sub_A_d8[3] = {
            Operands::readLSB,
            Operands::sub_A_d,
            nullptr
    };
    const Operand sbc_A_d8[3] = {
            Operands::readLSB,
            Operands::sbc_A_d,
            nullptr
    };
    const Operand sub_HL[3] = {
            Operands::_pad_,
            Operands::sub_HL,
            nullptr
    };
    const Operand sbc_HL[3] = {
            Operands::_pad_,
            Operands::sbc_A_HL,
            nullptr
    };

    const Operand jp_N_Z_a16[5] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::checkIsZeroContextual,
            Operands::jp,
            nullptr
    };
    const Operand jp_N_C_a16[5] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::checkIsCarryContextual,
            Operands::jp,
            nullptr
    };
    const Operand jp[5] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::_pad_,
            Operands::jp,
            nullptr
    };
    const Operand jp_HL[2] = {
            Operands::jp_HL,
            nullptr
    };

    const Operand jr_N_Z_r8[4] = {
            Operands::readSigned,
            Operands::checkIsZeroContextual,
            Operands::jr,
            nullptr
    };
    const Operand jr_N_C_r8[4] = {
            Operands::readSigned,
            Operands::checkIsCarryContextual,
            Operands::jr,
            nullptr
    };
    const Operand jr[4] = {
            Operands::readSigned,
            Operands::_pad_,
            Operands::jr,
            nullptr
    };

    const Operand call_N_Z_a16[7] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::checkIsZeroContextual,
            // Pad artificially to 6 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::call,
            nullptr
    };
    const Operand call_N_C_a16[7] = {
            Operands::readLSB,
            Operands::readMSB,
            Operands::checkIsCarryContextual,
            // Pad artificially to 6 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::call,
            nullptr
    };
    const Operand call_a16[7] = {
            Operands::readLSB,
            Operands::readMSB,
            // Pad artificially to 6 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::call,
            nullptr
    };

    const Operand ret_N_Z[6] = {
            // Pad artificially to 5 machine cycles TODO: do something useful here
            Operands::_pad_, // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            Operands::checkIsZeroContextual,
            Operands::_pad_,
            Operands::_pad_,
            Operands::ret,
            nullptr
    };
    const Operand ret_N_C[6] = {
            // Pad artificially to 5 machine cycles TODO: do something useful here
            Operands::_pad_, // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            Operands::checkIsCarryContextual,
            Operands::_pad_,
            Operands::_pad_,
            Operands::ret,
            nullptr
    };
    const Operand ret[5] = {
            // Pad artificially to 4 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            Operands::_pad_,
            Operands::ret,
            nullptr
    };

    const Operand reti[5] = {
            Operands::enableInterruptsImmediately,
            // Pad artificially to 4 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::ret,
            nullptr
    };

    const Operand rst_vec[5] = {
            // Pad artificially to 4 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::rst,
            nullptr
    };

    const Operand cp_r[2] = {
            Operands::cp_r,
            nullptr
    };
    const Operand cp_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::cp_HL,
            nullptr
    };
    const Operand cp_d8[3] = {
            Operands::readLSB,
            Operands::cp_d,
            nullptr
    };

    const Operand inc_ss[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::inc_ss,
            nullptr
    };
    const Operand inc_SP[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::inc_SP,
            nullptr
    };
    const Operand inc_HL[4] = {
            // Pad artificially to 3 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::inc_HL,
            nullptr
    };
    const Operand inc_r[2] = {
            Operands::inc_r,
            nullptr
    };

    const Operand dec_ss[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::dec_ss,
            nullptr
    };
    const Operand dec_SP[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::dec_SP,
            nullptr
    };
    const Operand dec_HL[4] = {
            // Pad artificially to 3 machine cycles TODO: do something useful here
            Operands::_pad_,
            Operands::_pad_,
            //
            Operands::dec_HL,
            nullptr
    };
    const Operand dec_r[2] = {
            Operands::dec_r,
            nullptr
    };

    const Operand or_r[2] = {
            Operands::or_r,
            nullptr
    };
    const Operand or_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::or_HL,
            nullptr
    };
    const Operand or_d8[3] = {
            Operands::readLSB,
            Operands::or_d,
            nullptr
    };

    const Operand and_r[2] = {
            Operands::and_r,
            nullptr
    };
    const Operand and_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::and_HL,
            nullptr
    };
    const Operand and_d8[3] = {
            Operands::readLSB,
            Operands::and_d,
            nullptr
    };

    const Operand xor_r[2] = {
            Operands::xor_r,
            nullptr
    };
    const Operand xor_HL[3] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::xor_HL,
            nullptr
    };
    const Operand xor_d8[3] = {
            Operands::readLSB,
            Operands::xor_d,
            nullptr
    };

    const Operand rrca[2] = {
            Operands::rrca,
            nullptr
    };

    const Operand rlca[2] = {
            Operands::rlca,
            nullptr
    };

    const Operand rra[2] = {
            Operands::rra,
            nullptr
    };

    const Operand rla[2] = {
            Operands::rla,
            nullptr
    };

    const Operand pop_rr[4] = {
            Operands::readStackIntoLSB,
            Operands::readStackIntoMSB,
            Operands::write16BitsIntoRR,
            nullptr
    };
    const Operand pop_AF[4] = {
            Operands::readStackIntoLSB,
            Operands::readStackIntoMSB,
            Operands::write16BitsIntoAF,
            nullptr
    };

    const Operand push_rr[5] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::readRRMSBIntoStack,
            Operands::readRRLSBIntoStack,
            Operands::_pad_, // TODO: do something useful here
            nullptr
    };
    const Operand push_AF[5] = {
            Operands::_pad_, // TODO: do something useful here
            Operands::readRegAIntoStack,
            Operands::readRegFIntoStack,
            Operands::_pad_, // TODO: do something useful here
            nullptr
    };

    const Operand daa[2] = {
            Operands::daa,
            nullptr
    };

    const Operand cpl[2] = {
            Operands::cpl,
            nullptr
    };

    const Operand scf[2] = {
            Operands::scf,
            nullptr
    };

    const Operand ccf[2] = {
            Operands::ccf,
            nullptr
    };

    const Operand di[2] = {
            Operands::disableInterrupts,
            nullptr
    };

    const Operand ei[2] = {
            Operands::enableInterruptsDelayed,
            nullptr
    };

    const Operand stop[2] = {
            Operands::stop,
            nullptr
    };

    const Operand halt[2] = {
            Operands::halt,
            nullptr
    };

    const Operand decodePrefix[3] = {
            Operands::decodePrefix,
            // This is a workaround, the decodePrefix operand will fetch the effective operand list to use
            // When changing this, please also adapt CPU::serialize and CPU::deserialize
            Operands::prefixPlaceholder,
            nullptr
    };
}

#endif
