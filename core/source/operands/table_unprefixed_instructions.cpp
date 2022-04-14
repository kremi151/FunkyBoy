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

#include "tables.h"

#include <operands/registry.h>

namespace FunkyBoy::Operands::Tables {

    Operand const* const instructions[256] = {
            Operands::Registry::nop,        // 0x00 NOP
            Operands::Registry::ld_ss_d16,  // 0x01 LD BC,u16
            Operands::Registry::ld_ss_A,    // 0x02 LD (BC),A
            Operands::Registry::inc_ss,     // 0x03 INC BC
            Operands::Registry::inc_r,      // 0x04 INC B
            Operands::Registry::dec_r,      // 0x05 DEC B
            Operands::Registry::ld_r_d8,    // 0x06 LD B,u8
            Operands::Registry::rlca,       // 0x07 RLCA
            Operands::Registry::ld_a16_SP,  // 0x08 LD (u16),SP
            Operands::Registry::add_HL_ss,  // 0x09 ADD HL,BC
            Operands::Registry::ld_A_ss,    // 0x0A LD A,(BC)
            Operands::Registry::dec_ss,     // 0x0B DEC BC
            Operands::Registry::inc_r,      // 0x0C INC C
            Operands::Registry::dec_r,      // 0x0D DEC C
            Operands::Registry::ld_r_d8,    // 0x0E LD C,u8
            Operands::Registry::rrca,       // 0x0F RRCA
            Operands::Registry::stop,       // 0x10 STOP
            Operands::Registry::ld_ss_d16,  // 0x11 LD DE,u16
            Operands::Registry::ld_ss_A,    // 0x12 LD (DE),A
            Operands::Registry::inc_ss,     // 0x13 INC DE
            Operands::Registry::inc_r,      // 0x14 INC D
            Operands::Registry::dec_r,      // 0x15 DEC D
            Operands::Registry::ld_r_d8,    // 0x16 LD D,u8
            Operands::Registry::rla,        // 0x17 RLA
            Operands::Registry::jr, // 0x18 JR i8
            Operands::Registry::add_HL_ss,  // 0x19 ADD HL,DE
            Operands::Registry::ld_A_ss,    // 0x1A LD A,(DE)
            Operands::Registry::dec_ss,     // 0x1B DEC DE
            Operands::Registry::inc_r,      // 0x1C INC E
            Operands::Registry::dec_r,      // 0x1D DEC E
            Operands::Registry::ld_r_d8,    // 0x1E LD E,u8
            Operands::Registry::rra,        // 0x1F RRA
            Operands::Registry::jr_N_Z_r8,  // 0x20 JR NZ,i8
            Operands::Registry::ld_ss_d16,  // 0x21 LD HL,u16
            Operands::Registry::ld_HLI_A,   // 0x22 LD (HL+),A
            Operands::Registry::inc_ss,     // 0x23 INC HL
            Operands::Registry::inc_r,      // 0x24 INC H
            Operands::Registry::dec_r,      // 0x25 DEC H
            Operands::Registry::ld_r_d8,    // 0x26 LD H,u8
            Operands::Registry::daa,        // 0x27 DAA
            Operands::Registry::jr_N_Z_r8,  // 0x28 JR Z,i8
            Operands::Registry::add_HL_ss,  // 0x29 ADD HL,HL
            Operands::Registry::ld_A_HLI,   // 0x2A LD A,(HL+)
            Operands::Registry::dec_ss,     // 0x2B DEC HL
            Operands::Registry::inc_r,      // 0x2C INC L
            Operands::Registry::dec_r,      // 0x2D DEC L
            Operands::Registry::ld_r_d8,    // 0x2E LD L,u8
            Operands::Registry::cpl,        // 0x2F CPL
            Operands::Registry::jr_N_C_r8,  // 0x30 JR NC,i8
            Operands::Registry::ld_SP_d16,  // 0x31 LD SP,u16
            Operands::Registry::ld_HLD_A,   // 0x32 LD (HL-),A
            Operands::Registry::inc_SP,     // 0x33 INC SP
            Operands::Registry::inc_HL,     // 0x34 INC (HL)
            Operands::Registry::dec_HL,     // 0x35 DEC (HL)
            Operands::Registry::ld_HL_d8,   // 0x36 LD (HL),u8
            Operands::Registry::scf,        // 0x37 SCF
            Operands::Registry::jr_N_C_r8,  // 0x38 JR C,i8
            Operands::Registry::add_HL_SP,  // 0x39 ADD HL,SP
            Operands::Registry::ld_A_HLD,   // 0x3A LD A,(HL-)
            Operands::Registry::dec_SP,     // 0x3B DEC SP
            Operands::Registry::inc_r,      // 0x3C INC A
            Operands::Registry::dec_r,      // 0x3D DEC A
            Operands::Registry::ld_A_d8,    // 0x3E LD A,u8
            Operands::Registry::ccf,        // 0x3F CCF
            Operands::Registry::ld_r_r,     // 0x40 LD B,B
            Operands::Registry::ld_r_r,     // 0x41 LD B,C
            Operands::Registry::ld_r_r,     // 0x42 LD B,D
            Operands::Registry::ld_r_r,     // 0x43 LD B,E
            Operands::Registry::ld_r_r,     // 0x44 LD B,H
            Operands::Registry::ld_r_r,     // 0x45 LD B,L
            Operands::Registry::ld_r_HL,    // 0x46 LD B,(HL)
            Operands::Registry::ld_r_r,     // 0x47 LD B,A
            Operands::Registry::ld_r_r,     // 0x48 LD C,B
            Operands::Registry::ld_r_r,     // 0x49 LD C,C
            Operands::Registry::ld_r_r,     // 0x4A LD C,D
            Operands::Registry::ld_r_r,     // 0x4B LD C,E
            Operands::Registry::ld_r_r,     // 0x4C LD C,H
            Operands::Registry::ld_r_r,     // 0x4D LD C,L
            Operands::Registry::ld_r_HL,    // 0x4E LD C,(HL)
            Operands::Registry::ld_r_r,     // 0x4F LD C,A
            Operands::Registry::ld_r_r,     // 0x50 LD D,B
            Operands::Registry::ld_r_r,     // 0x51 LD D,C
            Operands::Registry::ld_r_r,     // 0x52 LD D,D
            Operands::Registry::ld_r_r,     // 0x53 LD D,E
            Operands::Registry::ld_r_r,     // 0x54 LD D,H
            Operands::Registry::ld_r_r,     // 0x55 LD D,L
            Operands::Registry::ld_r_HL,    // 0x56 LD D,(HL)
            Operands::Registry::ld_r_r,     // 0x57 LD D,A
            Operands::Registry::ld_r_r,     // 0x58 LD E,B
            Operands::Registry::ld_r_r,     // 0x59 LD E,C
            Operands::Registry::ld_r_r,     // 0x5A LD E,D
            Operands::Registry::ld_r_r,     // 0x5B LD E,E
            Operands::Registry::ld_r_r,     // 0x5C LD E,H
            Operands::Registry::ld_r_r,     // 0x5D LD E,L
            Operands::Registry::ld_r_HL,    // 0x5E LD E,(HL)
            Operands::Registry::ld_r_r,     // 0x5F LD E,A
            Operands::Registry::ld_r_r,     // 0x60 LD H,B
            Operands::Registry::ld_r_r,     // 0x61 LD H,C
            Operands::Registry::ld_r_r,     // 0x62 LD H,D
            Operands::Registry::ld_r_r,     // 0x63 LD H,E
            Operands::Registry::ld_r_r,     // 0x64 LD H,H
            Operands::Registry::ld_r_r,     // 0x65 LD H,L
            Operands::Registry::ld_r_HL,    // 0x66 LD H,(HL)
            Operands::Registry::ld_r_r,     // 0x67 LD H,A
            Operands::Registry::ld_r_r,     // 0x68 LD L,B
            Operands::Registry::ld_r_r,     // 0x69 LD L,C
            Operands::Registry::ld_r_r,     // 0x6A LD L,D
            Operands::Registry::ld_r_r,     // 0x6B LD L,E
            Operands::Registry::ld_r_r,     // 0x6C LD L,H
            Operands::Registry::ld_r_r,     // 0x6D LD L,L
            Operands::Registry::ld_r_HL,    // 0x6E LD L,(HL)
            Operands::Registry::ld_r_r,     // 0x6F LD L,A
            Operands::Registry::ld_HL_r,    // 0x70 LD (HL),B
            Operands::Registry::ld_HL_r,    // 0x71 LD (HL),C
            Operands::Registry::ld_HL_r,    // 0x72 LD (HL),D
            Operands::Registry::ld_HL_r,    // 0x73 LD (HL),E
            Operands::Registry::ld_HL_r,    // 0x74 LD (HL),H
            Operands::Registry::ld_HL_r,    // 0x75 LD (HL),L
            Operands::Registry::halt,       // 0x76 HALT
            Operands::Registry::ld_HL_r,    // 0x77 LD (HL),A
            Operands::Registry::ld_r_r,     // 0x78 LD A,B
            Operands::Registry::ld_r_r,     // 0x79 LD A,C
            Operands::Registry::ld_r_r,     // 0x7A LD A,D
            Operands::Registry::ld_r_r,     // 0x7B LD A,E
            Operands::Registry::ld_r_r,     // 0x7C LD A,H
            Operands::Registry::ld_r_r,     // 0x7D LD A,L
            Operands::Registry::ld_r_HL,    // 0x7E LD A,(HL)
            Operands::Registry::ld_r_r,     // 0x7F LD A,A
            Operands::Registry::add_A_r,    // 0x80 ADD A,B
            Operands::Registry::add_A_r,    // 0x81 ADD A,C
            Operands::Registry::add_A_r,    // 0x82 ADD A,D
            Operands::Registry::add_A_r,    // 0x83 ADD A,E
            Operands::Registry::add_A_r,    // 0x84 ADD A,H
            Operands::Registry::add_A_r,    // 0x85 ADD A,L
            Operands::Registry::add_A_HL,   // 0x86 ADD A,(HL)
            Operands::Registry::add_A_r,    // 0x87 ADD A,A
            Operands::Registry::adc_A_r,    // 0x88 ADC A,B
            Operands::Registry::adc_A_r,    // 0x89 ADC A,C
            Operands::Registry::adc_A_r,    // 0x8A ADC A,D
            Operands::Registry::adc_A_r,    // 0x8B ADC A,E
            Operands::Registry::adc_A_r,    // 0x8C ADC A,H
            Operands::Registry::adc_A_r,    // 0x8D ADC A,L
            Operands::Registry::adc_A_HL,   // 0x8E ADC A,(HL)
            Operands::Registry::adc_A_r,    // 0x8F ADC A,A
            Operands::Registry::sub_A_r,    // 0x90 SUB A,B
            Operands::Registry::sub_A_r,    // 0x91 SUB A,C
            Operands::Registry::sub_A_r,    // 0x92 SUB A,D
            Operands::Registry::sub_A_r,    // 0x93 SUB A,E
            Operands::Registry::sub_A_r,    // 0x94 SUB A,H
            Operands::Registry::sub_A_r,    // 0x95 SUB A,L
            Operands::Registry::sub_HL,     // 0x96 SUB A,(HL)
            Operands::Registry::sub_A_r,    // 0x97 SUB A,A
            Operands::Registry::sbc_A_r,    // 0x98 SBC A,B
            Operands::Registry::sbc_A_r,    // 0x99 SBC A,C
            Operands::Registry::sbc_A_r,    // 0x9A SBC A,D
            Operands::Registry::sbc_A_r,    // 0x9B SBC A,E
            Operands::Registry::sbc_A_r,    // 0x9C SBC A,H
            Operands::Registry::sbc_A_r,    // 0x9D SBC A,L
            Operands::Registry::sbc_HL,     // 0x9E SBC A,(HL)
            Operands::Registry::sbc_A_r,    // 0x9F SBC A,A
            Operands::Registry::and_r,      // 0xA0 AND A,B
            Operands::Registry::and_r,      // 0xA1 AND A,C
            Operands::Registry::and_r,      // 0xA2 AND A,D
            Operands::Registry::and_r,      // 0xA3 AND A,E
            Operands::Registry::and_r,      // 0xA4 AND A,H
            Operands::Registry::and_r,      // 0xA5 AND A,L
            Operands::Registry::and_HL,     // 0xA6 AND A,(HL)
            Operands::Registry::and_r,      // 0xA7 AND A,A
            Operands::Registry::xor_r,      // 0xA8 XOR A,B
            Operands::Registry::xor_r,      // 0xA9 XOR A,C
            Operands::Registry::xor_r,      // 0xAA XOR A,D
            Operands::Registry::xor_r,      // 0xAB XOR A,E
            Operands::Registry::xor_r,      // 0xAC XOR A,H
            Operands::Registry::xor_r,      // 0xAD XOR A,L
            Operands::Registry::xor_HL,     // 0xAE XOR A,(HL)
            Operands::Registry::xor_r,      // 0xAF XOR A,A
            Operands::Registry::or_r,       // 0xB0 OR A,B
            Operands::Registry::or_r,       // 0xB1 OR A,C
            Operands::Registry::or_r,       // 0xB2 OR A,D
            Operands::Registry::or_r,       // 0xB3 OR A,E
            Operands::Registry::or_r,       // 0xB4 OR A,H
            Operands::Registry::or_r,       // 0xB5 OR A,L
            Operands::Registry::or_HL,      // 0xB6 OR A,(HL)
            Operands::Registry::or_r,       // 0xB7 OR A,A
            Operands::Registry::cp_r,       // 0xB8 CP A,B
            Operands::Registry::cp_r,       // 0xB9 CP A,C
            Operands::Registry::cp_r,       // 0xBA CP A,D
            Operands::Registry::cp_r,       // 0xBB CP A,E
            Operands::Registry::cp_r,       // 0xBC CP A,H
            Operands::Registry::cp_r,       // 0xBD CP A,L
            Operands::Registry::cp_HL,      // 0xBE CP A,(HL)
            Operands::Registry::cp_r,       // 0xBF CP A,A
            Operands::Registry::ret_N_Z,    // 0xC0 RET NZ
            Operands::Registry::pop_rr,     // 0xC1 POP BC
            Operands::Registry::jp_N_Z_a16, // 0xC2 JP NZ,u16
            Operands::Registry::jp, // 0xC3 JP u16
            Operands::Registry::call_N_Z_a16,       // 0xC4 CALL NZ,u16
            Operands::Registry::push_rr,    // 0xC5 PUSH BC
            Operands::Registry::add_A_d8,   // 0xC6 ADD A,u8
            Operands::Registry::rst_vec,    // 0xC7 RST 00h
            Operands::Registry::ret_N_Z,    // 0xC8 RET Z
            Operands::Registry::ret,        // 0xC9 RET
            Operands::Registry::jp_N_Z_a16, // 0xCA JP Z,u16
            Operands::Registry::decodePrefix,       // 0xCB PREFIX CB
            Operands::Registry::call_N_Z_a16,       // 0xCC CALL Z,u16
            Operands::Registry::call_a16,   // 0xCD CALL u16
            Operands::Registry::adc_A_d8,   // 0xCE ADC A,u8
            Operands::Registry::rst_vec,    // 0xCF RST 08h
            Operands::Registry::ret_N_C,    // 0xD0 RET NC
            Operands::Registry::pop_rr,     // 0xD1 POP DE
            Operands::Registry::jp_N_C_a16, // 0xD2 JP NC,u16
            nullptr,        // 0xD3
            Operands::Registry::call_N_C_a16,       // 0xD4 CALL NC,u16
            Operands::Registry::push_rr,    // 0xD5 PUSH DE
            Operands::Registry::sub_A_d8,   // 0xD6 SUB A,u8
            Operands::Registry::rst_vec,    // 0xD7 RST 10h
            Operands::Registry::ret_N_C,    // 0xD8 RET C
            Operands::Registry::reti,       // 0xD9 RETI
            Operands::Registry::jp_N_C_a16, // 0xDA JP C,u16
            nullptr,        // 0xDB
            Operands::Registry::call_N_C_a16,       // 0xDC CALL C,u16
            nullptr,        // 0xDD
            Operands::Registry::sbc_A_d8,   // 0xDE SBC A,u8
            Operands::Registry::rst_vec,    // 0xDF RST 18h
            Operands::Registry::ldh_a8_A,   // 0xE0 LD (FF00+u8),A
            Operands::Registry::pop_rr,     // 0xE1 POP HL
            Operands::Registry::ld_C_A,     // 0xE2 LD (FF00+C),A
            nullptr,        // 0xE3
            nullptr,        // 0xE4
            Operands::Registry::push_rr,    // 0xE5 PUSH HL
            Operands::Registry::and_d8,     // 0xE6 AND A,u8
            Operands::Registry::rst_vec,    // 0xE7 RST 20h
            Operands::Registry::add_SP_r8,  // 0xE8 ADD SP,i8
            Operands::Registry::jp_HL,      // 0xE9 JP HL
            Operands::Registry::ld_a16_A,   // 0xEA LD (u16),A
            nullptr,        // 0xEB
            nullptr,        // 0xEC
            nullptr,        // 0xED
            Operands::Registry::xor_d8,     // 0xEE XOR A,u8
            Operands::Registry::rst_vec,    // 0xEF RST 28h
            Operands::Registry::ldh_A_a8,   // 0xF0 LD A,(FF00+u8)
            Operands::Registry::pop_AF,     // 0xF1 POP AF
            Operands::Registry::ld_A_C,     // 0xF2 LD A,(FF00+C)
            Operands::Registry::di, // 0xF3 DI
            nullptr,        // 0xF4
            Operands::Registry::push_AF,    // 0xF5 PUSH AF
            Operands::Registry::or_d8,      // 0xF6 OR A,u8
            Operands::Registry::rst_vec,    // 0xF7 RST 30h
            Operands::Registry::ld_HL_SP_e8,        // 0xF8 LD HL,SP+i8
            Operands::Registry::ld_SP_HL,   // 0xF9 LD SP,HL
            Operands::Registry::ld_A_a16,   // 0xFA LD A,(u16)
            Operands::Registry::ei, // 0xFB EI
            nullptr,        // 0xFC
            nullptr,        // 0xFD
            Operands::Registry::cp_d8,      // 0xFE CP A,u8
            Operands::Registry::rst_vec,    // 0xFF RST 38h
    };

}

#endif
