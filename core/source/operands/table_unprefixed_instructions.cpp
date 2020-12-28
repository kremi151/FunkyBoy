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

#include "tables.h"

#include <operands/registry.h>

namespace FunkyBoy::Operands::Tables {

    Operand const* const instructions[256] = {
            Operands::Registry::nop,        // NOP
            Operands::Registry::ld_ss_d16,  // LD BC,u16
            Operands::Registry::ld_ss_A,    // LD (BC),A
            Operands::Registry::inc_ss,     // INC BC
            Operands::Registry::inc_r,      // INC B
            Operands::Registry::dec_r,      // DEC B
            Operands::Registry::ld_r_d8,    // LD B,u8
            Operands::Registry::rlca,       // RLCA
            Operands::Registry::ld_a16_SP,  // LD (u16),SP
            Operands::Registry::add_HL_ss,  // ADD HL,BC
            Operands::Registry::ld_A_ss,    // LD A,(BC)
            Operands::Registry::dec_ss,     // DEC BC
            Operands::Registry::inc_r,      // INC C
            Operands::Registry::dec_r,      // DEC C
            Operands::Registry::ld_r_d8,    // LD C,u8
            Operands::Registry::rrca,       // RRCA
            Operands::Registry::stop,       // STOP
            Operands::Registry::ld_ss_d16,  // LD DE,u16
            Operands::Registry::ld_ss_A,    // LD (DE),A
            Operands::Registry::inc_ss,     // INC DE
            Operands::Registry::inc_r,      // INC D
            Operands::Registry::dec_r,      // DEC D
            Operands::Registry::ld_r_d8,    // LD D,u8
            Operands::Registry::rla,        // RLA
            Operands::Registry::jr, // JR i8
            Operands::Registry::add_HL_ss,  // ADD HL,DE
            Operands::Registry::ld_A_ss,    // LD A,(DE)
            Operands::Registry::dec_ss,     // DEC DE
            Operands::Registry::inc_r,      // INC E
            Operands::Registry::dec_r,      // DEC E
            Operands::Registry::ld_r_d8,    // LD E,u8
            Operands::Registry::rra,        // RRA
            Operands::Registry::jr_N_Z_r8,  // JR NZ,i8
            Operands::Registry::ld_ss_d16,  // LD HL,u16
            Operands::Registry::ld_HLI_A,   // LD (HL+),A
            Operands::Registry::inc_ss,     // INC HL
            Operands::Registry::inc_r,      // INC H
            Operands::Registry::dec_r,      // DEC H
            Operands::Registry::ld_r_d8,    // LD H,u8
            Operands::Registry::daa,        // DAA
            Operands::Registry::jr_N_Z_r8,  // JR Z,i8
            Operands::Registry::add_HL_ss,  // ADD HL,HL
            Operands::Registry::ld_A_HLI,   // LD A,(HL+)
            Operands::Registry::dec_ss,     // DEC HL
            Operands::Registry::inc_r,      // INC L
            Operands::Registry::dec_r,      // DEC L
            Operands::Registry::ld_r_d8,    // LD L,u8
            Operands::Registry::cpl,        // CPL
            Operands::Registry::jr_N_C_r8,  // JR NC,i8
            Operands::Registry::ld_SP_d16,  // LD SP,u16
            Operands::Registry::ld_HLD_A,   // LD (HL-),A
            Operands::Registry::inc_SP,     // INC SP
            Operands::Registry::inc_HL,     // INC (HL)
            Operands::Registry::dec_HL,     // DEC (HL)
            Operands::Registry::ld_HL_d8,   // LD (HL),u8
            Operands::Registry::scf,        // SCF
            Operands::Registry::jr_N_C_r8,  // JR C,i8
            Operands::Registry::add_HL_SP,  // ADD HL,SP
            Operands::Registry::ld_A_HLD,   // LD A,(HL-)
            Operands::Registry::dec_SP,     // DEC SP
            Operands::Registry::inc_r,      // INC A
            Operands::Registry::dec_r,      // DEC A
            Operands::Registry::ld_A_d8,    // LD A,u8
            Operands::Registry::ccf,        // CCF
            Operands::Registry::ld_r_r,     // LD B,B
            Operands::Registry::ld_r_r,     // LD B,C
            Operands::Registry::ld_r_r,     // LD B,D
            Operands::Registry::ld_r_r,     // LD B,E
            Operands::Registry::ld_r_r,     // LD B,H
            Operands::Registry::ld_r_r,     // LD B,L
            Operands::Registry::ld_r_HL,    // LD B,(HL)
            Operands::Registry::ld_r_r,     // LD B,A
            Operands::Registry::ld_r_r,     // LD C,B
            Operands::Registry::ld_r_r,     // LD C,C
            Operands::Registry::ld_r_r,     // LD C,D
            Operands::Registry::ld_r_r,     // LD C,E
            Operands::Registry::ld_r_r,     // LD C,H
            Operands::Registry::ld_r_r,     // LD C,L
            Operands::Registry::ld_r_HL,    // LD C,(HL)
            Operands::Registry::ld_r_r,     // LD C,A
            Operands::Registry::ld_r_r,     // LD D,B
            Operands::Registry::ld_r_r,     // LD D,C
            Operands::Registry::ld_r_r,     // LD D,D
            Operands::Registry::ld_r_r,     // LD D,E
            Operands::Registry::ld_r_r,     // LD D,H
            Operands::Registry::ld_r_r,     // LD D,L
            Operands::Registry::ld_r_HL,    // LD D,(HL)
            Operands::Registry::ld_r_r,     // LD D,A
            Operands::Registry::ld_r_r,     // LD E,B
            Operands::Registry::ld_r_r,     // LD E,C
            Operands::Registry::ld_r_r,     // LD E,D
            Operands::Registry::ld_r_r,     // LD E,E
            Operands::Registry::ld_r_r,     // LD E,H
            Operands::Registry::ld_r_r,     // LD E,L
            Operands::Registry::ld_r_HL,    // LD E,(HL)
            Operands::Registry::ld_r_r,     // LD E,A
            Operands::Registry::ld_r_r,     // LD H,B
            Operands::Registry::ld_r_r,     // LD H,C
            Operands::Registry::ld_r_r,     // LD H,D
            Operands::Registry::ld_r_r,     // LD H,E
            Operands::Registry::ld_r_r,     // LD H,H
            Operands::Registry::ld_r_r,     // LD H,L
            Operands::Registry::ld_r_HL,    // LD H,(HL)
            Operands::Registry::ld_r_r,     // LD H,A
            Operands::Registry::ld_r_r,     // LD L,B
            Operands::Registry::ld_r_r,     // LD L,C
            Operands::Registry::ld_r_r,     // LD L,D
            Operands::Registry::ld_r_r,     // LD L,E
            Operands::Registry::ld_r_r,     // LD L,H
            Operands::Registry::ld_r_r,     // LD L,L
            Operands::Registry::ld_r_HL,    // LD L,(HL)
            Operands::Registry::ld_r_r,     // LD L,A
            Operands::Registry::ld_HL_r,    // LD (HL),B
            Operands::Registry::ld_HL_r,    // LD (HL),C
            Operands::Registry::ld_HL_r,    // LD (HL),D
            Operands::Registry::ld_HL_r,    // LD (HL),E
            Operands::Registry::ld_HL_r,    // LD (HL),H
            Operands::Registry::ld_HL_r,    // LD (HL),L
            Operands::Registry::halt,       // HALT
            Operands::Registry::ld_HL_r,    // LD (HL),A
            Operands::Registry::ld_r_r,     // LD A,B
            Operands::Registry::ld_r_r,     // LD A,C
            Operands::Registry::ld_r_r,     // LD A,D
            Operands::Registry::ld_r_r,     // LD A,E
            Operands::Registry::ld_r_r,     // LD A,H
            Operands::Registry::ld_r_r,     // LD A,L
            Operands::Registry::ld_r_HL,    // LD A,(HL)
            Operands::Registry::ld_r_r,     // LD A,A
            Operands::Registry::add_A_r,    // ADD A,B
            Operands::Registry::add_A_r,    // ADD A,C
            Operands::Registry::add_A_r,    // ADD A,D
            Operands::Registry::add_A_r,    // ADD A,E
            Operands::Registry::add_A_r,    // ADD A,H
            Operands::Registry::add_A_r,    // ADD A,L
            Operands::Registry::add_A_HL,   // ADD A,(HL)
            Operands::Registry::add_A_r,    // ADD A,A
            Operands::Registry::adc_A_r,    // ADC A,B
            Operands::Registry::adc_A_r,    // ADC A,C
            Operands::Registry::adc_A_r,    // ADC A,D
            Operands::Registry::adc_A_r,    // ADC A,E
            Operands::Registry::adc_A_r,    // ADC A,H
            Operands::Registry::adc_A_r,    // ADC A,L
            Operands::Registry::adc_A_HL,   // ADC A,(HL)
            Operands::Registry::adc_A_r,    // ADC A,A
            Operands::Registry::sub_A_r,    // SUB A,B
            Operands::Registry::sub_A_r,    // SUB A,C
            Operands::Registry::sub_A_r,    // SUB A,D
            Operands::Registry::sub_A_r,    // SUB A,E
            Operands::Registry::sub_A_r,    // SUB A,H
            Operands::Registry::sub_A_r,    // SUB A,L
            Operands::Registry::sub_HL,     // SUB A,(HL)
            Operands::Registry::sub_A_r,    // SUB A,A
            Operands::Registry::sbc_A_r,    // SBC A,B
            Operands::Registry::sbc_A_r,    // SBC A,C
            Operands::Registry::sbc_A_r,    // SBC A,D
            Operands::Registry::sbc_A_r,    // SBC A,E
            Operands::Registry::sbc_A_r,    // SBC A,H
            Operands::Registry::sbc_A_r,    // SBC A,L
            Operands::Registry::sbc_HL,     // SBC A,(HL)
            Operands::Registry::sbc_A_r,    // SBC A,A
            Operands::Registry::and_r,      // AND A,B
            Operands::Registry::and_r,      // AND A,C
            Operands::Registry::and_r,      // AND A,D
            Operands::Registry::and_r,      // AND A,E
            Operands::Registry::and_r,      // AND A,H
            Operands::Registry::and_r,      // AND A,L
            Operands::Registry::and_HL,     // AND A,(HL)
            Operands::Registry::and_r,      // AND A,A
            Operands::Registry::xor_r,      // XOR A,B
            Operands::Registry::xor_r,      // XOR A,C
            Operands::Registry::xor_r,      // XOR A,D
            Operands::Registry::xor_r,      // XOR A,E
            Operands::Registry::xor_r,      // XOR A,H
            Operands::Registry::xor_r,      // XOR A,L
            Operands::Registry::xor_HL,     // XOR A,(HL)
            Operands::Registry::xor_r,      // XOR A,A
            Operands::Registry::or_r,       // OR A,B
            Operands::Registry::or_r,       // OR A,C
            Operands::Registry::or_r,       // OR A,D
            Operands::Registry::or_r,       // OR A,E
            Operands::Registry::or_r,       // OR A,H
            Operands::Registry::or_r,       // OR A,L
            Operands::Registry::or_HL,      // OR A,(HL)
            Operands::Registry::or_r,       // OR A,A
            Operands::Registry::cp_r,       // CP A,B
            Operands::Registry::cp_r,       // CP A,C
            Operands::Registry::cp_r,       // CP A,D
            Operands::Registry::cp_r,       // CP A,E
            Operands::Registry::cp_r,       // CP A,H
            Operands::Registry::cp_r,       // CP A,L
            Operands::Registry::cp_HL,      // CP A,(HL)
            Operands::Registry::cp_r,       // CP A,A
            Operands::Registry::ret_N_Z,    // RET NZ
            Operands::Registry::pop_rr,     // POP BC
            Operands::Registry::jp_N_Z_a16, // JP NZ,u16
            Operands::Registry::jp, // JP u16
            Operands::Registry::call_N_Z_a16,       // CALL NZ,u16
            Operands::Registry::push_rr,    // PUSH BC
            Operands::Registry::add_A_d8,   // ADD A,u8
            Operands::Registry::rst_vec,    // RST 00h
            Operands::Registry::ret_N_Z,    // RET Z
            Operands::Registry::ret,        // RET
            Operands::Registry::jp_N_Z_a16, // JP Z,u16
            Operands::Registry::decodePrefix,       // PREFIX CB
            Operands::Registry::call_N_Z_a16,       // CALL Z,u16
            Operands::Registry::call_a16,   // CALL u16
            Operands::Registry::adc_A_d8,   // ADC A,u8
            Operands::Registry::rst_vec,    // RST 08h
            Operands::Registry::ret_N_C,    // RET NC
            Operands::Registry::pop_rr,     // POP DE
            Operands::Registry::jp_N_C_a16, // JP NC,u16
            nullptr,
            Operands::Registry::call_N_C_a16,       // CALL NC,u16
            Operands::Registry::push_rr,    // PUSH DE
            Operands::Registry::sub_A_d8,   // SUB A,u8
            Operands::Registry::rst_vec,    // RST 10h
            Operands::Registry::ret_N_C,    // RET C
            Operands::Registry::reti,       // RETI
            Operands::Registry::jp_N_C_a16, // JP C,u16
            nullptr,
            Operands::Registry::call_N_C_a16,       // CALL C,u16
            nullptr,
            Operands::Registry::sbc_A_d8,   // SBC A,u8
            Operands::Registry::rst_vec,    // RST 18h
            Operands::Registry::ldh_a8_A,   // LD (FF00+u8),A
            Operands::Registry::pop_rr,     // POP HL
            Operands::Registry::ld_C_A,     // LD (FF00+C),A
            nullptr,
            nullptr,
            Operands::Registry::push_rr,    // PUSH HL
            Operands::Registry::and_d8,     // AND A,u8
            Operands::Registry::rst_vec,    // RST 20h
            Operands::Registry::add_SP_r8,  // ADD SP,i8
            Operands::Registry::jp_HL,      // JP HL
            Operands::Registry::ld_a16_A,   // LD (u16),A
            nullptr,
            nullptr,
            nullptr,
            Operands::Registry::xor_d8,     // XOR A,u8
            Operands::Registry::rst_vec,    // RST 28h
            Operands::Registry::ldh_A_a8,   // LD A,(FF00+u8)
            Operands::Registry::pop_AF,     // POP AF
            Operands::Registry::ld_A_C,     // LD A,(FF00+C)
            Operands::Registry::di, // DI
            nullptr,
            Operands::Registry::push_AF,    // PUSH AF
            Operands::Registry::or_d8,      // OR A,u8
            Operands::Registry::rst_vec,    // RST 30h
            Operands::Registry::ld_HL_SP_e8,        // LD HL,SP+i8
            Operands::Registry::ld_SP_HL,   // LD SP,HL
            Operands::Registry::ld_A_a16,   // LD A,(u16)
            Operands::Registry::ei, // EI
            nullptr,
            nullptr,
            Operands::Registry::cp_d8,      // CP A,u8
            Operands::Registry::rst_vec,    // RST 38h
    };

}