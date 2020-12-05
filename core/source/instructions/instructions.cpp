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

#include "instructions.h"

#include <instructions/loads.h>
#include <instructions/alu.h>
#include <instructions/jumps.h>
#include <instructions/rot_shifts.h>
#include <instructions/stack.h>
#include <instructions/misc.h>

#include <util/debug.h>

using namespace FunkyBoy;

int Instructions::execute(opcode_t opcode, Instructions::context &context, Memory &memory) {
    switch (opcode) {
        /* nop */ case 0x00:
        {
            debug_print_4("nop\n");
            // No-op
            return 4;
        }
        /* ld b,reg */ case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47:
        /* ld c,reg */ case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f:
        /* ld d,reg */ case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57:
        /* ld e,reg */ case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f:
        /* ld h,reg */ case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67:
        /* ld l,reg */ case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f:
        /* ld a,reg */ case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f:
        {
            debug_print_4("ld reg,reg\n");
            Loads::ld_reg_reg(opcode, context);
            return 4;
        }
        /* ld (a16),A */ case 0xEA: {
            debug_print_4("ld (a16),A\n");
            Loads::ld_a16_A(memory, context);
            return 16;
        }
        /* ld A,(a16) */ case 0xFA: {
            debug_print_4("ld A,(a16)\n");
            Loads::ld_A_a16(memory, context);
            return 16;
        }
        /* ld (C),A */ case 0xE2: {
            debug_print_4("ld (C),A\n");
            Loads::ld_C_A(memory, context);
            return 8;
        }
        /* ld A,(C) */ case 0xF2: {
            debug_print_4("ld A,(C)\n");
            Loads::ld_A_C(memory, context);
            return 8;
        }
        /* ld A,d8 */ case 0x3E: {
            debug_print_4("ld A,d8\n");
            Loads::ld_A_d8(memory, context);
            return 8;
        }
        /* ld (ss),d16 */ case 0x01: case 0x11: case 0x21: {
            debug_print_4("ld (ss),d16\n");
            Loads::ld_ss_d16(opcode, memory, context);
            return 12;
        }
        /* ld SP,d16 */ case 0x31: {
            debug_print_4("ld SP,d16\n");
            Loads::ld_SP_d16(memory, context);
            return 12;
        }
        /* ld (a16),SP */ case 0x08: {
            debug_print_4("ld (a16),SP\n");
            Loads::ld_a16_SP(memory, context);
            return 20;
        }
        /* ld r,d8 */ case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print_4("ld r,d8\n");
            Loads::ld_r_d8(opcode, memory, context);
            return 8;
        }
        /* ld (HL),d8 */ case 0x36: {
            debug_print_4("ld (HL),d8\n");
            Loads::ld_HL_d8(memory, context);
            return 12;
        }
        /* ld (ss),A */ case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            Loads::ld_ss_A(opcode, memory, context);
            return 8;
        }
        /* ld A,(ss) */ case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            Loads::ld_A_ss(opcode, memory, context);
            return 8;
        }
        /* ld (HLI),A */ case 0x22: {
            debug_print_4("ld (HLI),A\n");
            Loads::ld_HLI_A(memory, context);
            return 8;
        }
        /* ld (HLD),A */ case 0x32: {
            debug_print_4("ld (HLD),A\n");
            Loads::ld_HLD_A(memory, context);
            return 8;
        }
        /* ld A,(HLI) */ case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            Loads::ld_A_HLI(memory, context);
            return 8;
        }
        /* ld A,(HLD) */ case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            Loads::ld_A_HLD(memory, context);
            return 8;
        }
        /* ld (HL),r */ case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            debug_print_4("ld (HL),r\n");
            Loads::ld_HL_r(opcode, memory, context);
            return 8;
        }
        /* ld r,(HL) */ case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            debug_print_4("ld r,(HL)\n");
            Loads::ld_r_HL(opcode, memory, context);
            return 8;
        }
        /* ld SP,HL */ case 0xF9: {
            debug_print_4("ld SP,HL\n");
            Loads::ld_SP_HL(memory, context);
            return 8;
        }
        /* ld HL,SP+e8 */ case 0xF8: {
            debug_print_4("ld HL,SP+e8\n");
            Loads::ld_HL_SP_plus_e8(memory, context);
            return 12;
        }
        /* ldh (a8),A */ case 0xE0: {
            debug_print_4("ldh (a8),A\n");
            Loads::ldh_a8_A(memory, context);
            return 12;
        }
        /* ldh A,(a8) */ case 0xF0: {
            debug_print_4("ldh A,(a8)\n");
            Loads::ldh_A_a8(memory, context);
            return 12;
        }
        /* add a,reg */ case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: {
            debug_print_4("add A,r\n");
            ALU::add_A_r(opcode, memory, context);
            return 4;
        }
        /* adc a,reg */ case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: {
            debug_print_4("adc A,r\n");
            ALU::adc_A_r(opcode, memory, context);
            return 4;
        }
        /* add A,d8 */ case 0xC6: {
            debug_print_4("add A,d8\n");
            ALU::add_A_d8(memory, context);
            return 8;
        }
        /* adc A,d8 */ case 0xCE: {
            debug_print_4("adc A,d8\n");
            ALU::adc_A_d8(memory, context);
            return 8;
        }
        /* add HL,ss */ case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            ALU::add_HL_ss(opcode, memory, context);
            return 8;
        }
        /* add HL,SP */ case 0x39: {
            debug_print_4("add HL,SP\n");
            ALU::add_HL_SP(memory, context);
            return 8;
        }
        /* add SP,r8 */ case 0xE8: {
            debug_print_4("add SP,e8\n");
            ALU::add_SP_e8(memory, context);
            return 16;
        }
        /* add A,(HL) */ case 0x86: {
            debug_print_4("add A,(HL)\n");
            ALU::add_A_HL(memory, context);
            return 8;
        }
        /* adc A,(HL) */ case 0x8E: {
            debug_print_4("adc A,(HL)\n");
            ALU::adc_A_HL(memory, context);
            return 8;
        }
        /* sub a,reg */ case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: {
            debug_print_4("sub A,r\n");
            ALU::sub_A_r(opcode, memory, context);
            return 4;
        }
        /* sbc a,reg */ case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: {
            debug_print_4("sbc A,r\n");
            ALU::sbc_A_r(opcode, memory, context);
            return 4;
        }
        /* sub A,d8 */ case 0xD6: {
            debug_print_4("sub A,d8\n");
            ALU::sub_A_d8(memory, context);
            return 8;
        }
        /* sbc A,d8 */ case 0xDE: {
            debug_print_4("sbc A,d8\n");
            ALU::sbc_A_d8(memory, context);
            return 8;
        }
        /* sub (HL) */ case 0x96: {
            ALU::sub_HL(memory, context);
            return 8;
        }
        /* sbc (HL) */ case 0x9E: {
            ALU::sbc_A_HL(memory, context);
            return 8;
        }
        /* jp (N)Z,a16 */ case 0xC2: case 0xCA: {
            debug_print_4("jp (N)Z,a16\n");
            return Jumps::jp_NZ_a16(opcode, memory, context);
        }
        /* jp (N)C,a16 */ case 0xD2: case 0xDA: {
            debug_print_4("jp (N)C,a16\n");
            return Jumps::jp_NC_a16(opcode, memory, context);
        }
        /* jp a16 */ case 0xC3: {
            debug_print_4("jp a16\n");
            Jumps::jp_a16(memory, context);
            return 16;
        }
        /* jp HL */ case 0xE9: {
            debug_print_4("jp HL\n");
            Jumps::jp_HL(context);
            return 4;
        }
        /* jr (N)Z,r8 */ case 0x20: case 0x28: {
            debug_print_4("jr (N)Z,r8\n");
            return Jumps::jr_NZ_r8(opcode, memory, context);
        }
        /* jr (N)C,r8 */ case 0x30: case 0x38: {
            debug_print_4("jr (N)C,r8\n");
            return Jumps::jr_NC_r8(opcode, memory, context);
        }
        /* jr r8 */ case 0x18: {
            debug_print_4("jr r8\n");
            Jumps::jr_r8(memory, context);
            return 12;
        }
        /* call (N)Z,a16 */ case 0xC4: case 0xCC: {
            debug_print_4("call (N)Z,a16\n");
            return Jumps::call_NZ_a16(opcode, memory, context);
        }
        /* call (N)C,a16 */ case 0xD4: case 0xDC: {
            debug_print_4("call (N)C,a16\n");
            return Jumps::call_NC_a16(opcode, memory, context);
        }
        /* call a16 */ case 0xCD: {
            debug_print_4("call a16\n");
            Jumps::call_a16(memory, context);
            return 24;
        }
        /* ret (N)Z */ case 0xC0: case 0xC8: {
            debug_print_4("ret (N)Z\n");
            return Jumps::ret_NZ(opcode, memory, context);
        }
        /* ret (N)C */ case 0xD0: case 0xD8: {
            debug_print_4("ret (N)C\n");
            return Jumps::ret_NC(opcode, memory, context);
        }
        /* ret */ case 0xC9: {
            debug_print_4("ret\n");
            Jumps::ret(memory, context);
            return 16;
        }
        /* reti */ case 0xD9: {
            debug_print_4("reti\n");
            Jumps::reti(memory, context);
            return 16;
        }
        /* rst vec */ case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            debug_print_4("rst vec\n");
            Jumps::rst(opcode, memory, context);
            return 16;
        }
        /* cp r */ case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
            debug_print_4("cp r\n");
            ALU::cp_r(opcode, memory, context);
            return 4;
        }
        /* cp (HL) */ case 0xBE: {
            debug_print_4("cp (HL)\n");
            ALU::cp_HL(memory, context);
            return 8;
        }
        /* cp d8 */ case 0xFE: {
            debug_print_4("cp d8\n");
            ALU::cp_d8(memory, context);
            return 8;
        }
        /* inc ss */ case 0x03: case 0x13: case 0x23: {
            debug_print_4("inc ss\n");
            ALU::inc_ss(opcode, memory, context);
            return 8;
        }
        /* inc SP */ case 0x33: {
            debug_print_4("inc SP\n");
            ALU::inc_SP(context);
            return 8;
        }
        /* inc (HL) */ case 0x34: {
            debug_print_4("inc (HL)\n");
            ALU::inc_HL(memory, context);
            return 12;
        }
        /* inc r */ case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x3C: {
            debug_print_4("inc r\n");
            ALU::inc_r(opcode, memory, context);
            return 4;
        }
        /* dec ss */ case 0x0B: case 0x1B: case 0x2B: {
            debug_print_4("dec ss\n");
            ALU::dec_ss(opcode, memory, context);
            return 8;
        }
        /* dec SP */ case 0x3B: {
            debug_print_4("dec SP\n");
            ALU::dec_SP(context);
            return 8;
        }
        /* dec (HL) */ case 0x35: {
            debug_print_4("dec (HL)\n");
            ALU::dec_HL(memory, context);
            return 12;
        }
        /* dec r */ case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: case 0x3D: {
            debug_print_4("dec r\n");
            ALU::dec_r(opcode, memory, context);
            return 4;
        }
        /* or r */ case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: {
            debug_print_4("or r\n");
            ALU::or_r(opcode, memory, context);
            return 4;
        }
        /* or (HL) */ case 0xB6: {
            debug_print_4("or (HL)\n");
            ALU::or_HL(memory, context);
            return 8;
        }
        /* or d8 */ case 0xF6: {
            debug_print_4("or d\n");
            ALU::or_d8(memory, context);
            return 8;
        }
        /* and r */ case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: {
            debug_print_4("and r\n");
            ALU::and_r(opcode, memory, context);
            return 4;
        }
        /* and (HL) */ case 0xA6: {
            debug_print_4("and (HL)\n");
            ALU::and_HL(memory, context);
            return 8;
        }
        /* and d8 */ case 0xE6: {
            debug_print_4("and d\n");
            ALU::and_d8(memory, context);
            return 8;
        }
        /* xor r */ case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            debug_print_4("xor r\n");
            ALU::xor_r(opcode, memory, context);
            return 4;
        }
        /* xor (HL) */ case 0xAE: {
            debug_print_4("xor (HL)\n");
            ALU::xor_HL(memory, context);
            return 8;
        }
        /* xor d8 */ case 0xEE: {
            debug_print_4("xor d\n");
            ALU::xor_d8(memory, context);
            return 8;
        }
        /* rrca */ case 0x0F: {
            debug_print_4("rrca\n");
            RotShifts::rrca(context);
            return 4;
        }
        /* rlca */ case 0x07: {
            debug_print_4("rlca\n");
            RotShifts::rlca(context);
            return 4;
        }
        /* rra */ case 0x1F: {
            debug_print_4("rra\n");
            RotShifts::rra(context);
            return 4;
        }
        /* rla */ case 0x17: {
            debug_print_4("rla\n");
            RotShifts::rla(context);
            return 4;
        }
        /* pop rr */ case 0xC1: case 0xD1: case 0xE1: {
            debug_print_4("pop rr\n");
            Stack::pop_rr(opcode, memory, context);
            return 12;
        }
        /* pop AF */ case 0xF1: {
            debug_print_4("pop AF\n");
            Stack::pop_AF(memory, context);
            return 12;
        }
        /* push rr */ case 0xC5: case 0xD5: case 0xE5: {
            debug_print_4("push rr\n");
            Stack::push_rr(opcode, memory, context);
            return 16;
        }
        /* push AF */ case 0xF5: {
            debug_print_4("push AF\n");
            Stack::push_AF(memory, context);
            return 16;
        }
        /* daa */ case 0x27: {
            debug_print_4("daa\n");
            Miscellaneous::daa(context);
            return 4;
        }
        /* cpl */ case 0x2F: {
            debug_print_4("cpl\n");
            Miscellaneous::cpl(context);
            return 4;
        }
        /* scf */ case 0x37: {
            debug_print_4("scf\n");
            Miscellaneous::scf(context);
            return 4;
        }
        /* ccf */ case 0x3F: {
            debug_print_4("ccf\n");
            Miscellaneous::ccf(context);
            return 4;
        }
        default:
            return 0;
    }
}
