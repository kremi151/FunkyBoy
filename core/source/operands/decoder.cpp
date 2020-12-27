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

#include "decoder.h"

#include <operands/registry.h>
#include <util/debug.h>

using namespace FunkyBoy;

const Operand * Operands::decodeOpcode(u8 opcode) {
    switch (opcode) {
        // nop
        case 0x00: {
            debug_print_4("nop\n");
            return Operands::Registry::nop;
        }
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            debug_print_4("ld reg,reg\n");
            return Operands::Registry::ld_r_r;
        }
        // ld (a16),A
        case 0xEA: {
            debug_print_4("ld (a16),A\n");
            return Operands::Registry::ld_a16_A;
        }
        // ld A,(a16)
        case 0xFA: {
            debug_print_4("ld A,(a16)\n");
            return Operands::Registry::ld_A_a16;
        }
        // ld (C),A
        case 0xE2: {
            debug_print_4("ld (C),A\n");
            return Operands::Registry::ld_C_A;
        }
        // ld A,(C)
        case 0xF2: {
            debug_print_4("ld A,(C)\n");
            return Operands::Registry::ld_A_C;
        }
        // ld A,d8
        case 0x3E: {
            debug_print_4("ld A,d8\n");
            return Operands::Registry::ld_A_d8;
        }
        // ld (ss),d16
        case 0x01: case 0x11: case 0x21: {
            debug_print_4("ld (ss),d16\n");
            return Operands::Registry::ld_ss_d16;
        }
        // ld SP,d16
        case 0x31: {
            debug_print_4("ld SP,d16\n");
            return Operands::Registry::ld_SP_d16;
        }
        // ld (a16),SP
        case 0x08: {
            debug_print_4("ld (a16),SP\n");
            return Operands::Registry::ld_a16_SP;
        }
        // ld r,d8
        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print_4("ld r,d8\n");
            return Operands::Registry::ld_r_d8;
        }
        // ld (HL),d8
        case 0x36: {
            debug_print_4("ld (HL),d8\n");
            return Operands::Registry::ld_HL_d8;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            return Operands::Registry::ld_ss_A;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            return Operands::Registry::ld_A_ss;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            return Operands::Registry::ld_HLI_A;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            return Operands::Registry::ld_HLD_A;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            return Operands::Registry::ld_A_HLI;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            return Operands::Registry::ld_A_HLD;
        }
        // ld (HL),r
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            debug_print_4("ld (HL),r\n");
            return Operands::Registry::ld_HL_r;
        }
        // ld r,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            debug_print_4("ld r,(HL)\n");
            return Operands::Registry::ld_r_HL;
        }
        // ld SP,HL
        case 0xF9: {
            debug_print_4("ld SP,HL\n");
            return Operands::Registry::ld_SP_HL;
        }
        // ld HL,SP+e8
        case 0xF8: {
            debug_print_4("ld HL,SP+e8\n");
            return Operands::Registry::ld_HL_SP_e8;
        }
        // ldh (a8),A
        case 0xE0: {
            debug_print_4("ldh (a8),A\n");
            return Operands::Registry::ldh_a8_A;
        }
        // ldh A,(a8)
        case 0xF0: {
            debug_print_4("ldh A,(a8)\n");
            return Operands::Registry::ldh_A_a8;
        }
        // add a,reg
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: {
            debug_print_4("add A,r\n");
            return Operands::Registry::add_A_r;
        }
        // adc a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: {
            debug_print_4("adc A,r\n");
            return Operands::Registry::adc_A_r;
        }
        // add A,d8
        case 0xC6: {
            debug_print_4("add A,d8\n");
            return Operands::Registry::add_A_d8;
        }
        // adc A,d8
        case 0xCE: {
            debug_print_4("add A,d8\n");
            return Operands::Registry::adc_A_d8;
        }
        // add HL,ss
        case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            return Operands::Registry::add_HL_ss;
        }
        // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            return Operands::Registry::add_HL_SP;
        }
        // add SP,r8
        case 0xE8: {
            debug_print_4("add SP,e8\n");
            return Operands::Registry::add_SP_r8;
        }
        // add A,(HL)
        case 0x86: {
            debug_print_4("add A,(HL)\n");
            return Operands::Registry::add_A_HL;
        }
        // adc A,(HL)
        case 0x8E: {
            debug_print_4("adc A,(HL)\n");
            return Operands::Registry::adc_A_HL;
        }
        // sub a,reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: {
            debug_print_4("sub A,r\n");
            return Operands::Registry::sub_A_r;
        }
        // sbc a,reg
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: {
            debug_print_4("sbc A,r\n");
            return Operands::Registry::sbc_A_r;
        }
        // sub A,d8
        case 0xD6: {
            debug_print_4("sub A,d8\n");
            return Operands::Registry::sub_A_d8;
        }
        // sbc A,d8
        case 0xDE: {
            debug_print_4("sbc A,d8\n");
            return Operands::Registry::sbc_A_d8;
        }
        // sub (HL)
        case 0x96: {
            return Operands::Registry::sub_HL;
        }
        // sbc (HL)
        case 0x9E: {
            return Operands::Registry::sbc_HL;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            debug_print_4("jp (N)Z,a16\n");
            return Operands::Registry::jp_N_Z_a16;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            debug_print_4("jp (N)C,a16\n");
            return Operands::Registry::jp_N_C_a16;
        }
        // unconditional jp
        case 0xC3: {
            debug_print_4("jp a16\n");
            return Operands::Registry::jp;
        }
        // jp HL
        case 0xE9: {
            debug_print_4("jp HL\n");
            return Operands::Registry::jp_HL;
        }
        // jr (N)Z,r8
        case 0x20: case 0x28: {
            debug_print_4("jr (N)Z,r8\n");
            return Operands::Registry::jr_N_Z_a16;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: {
            debug_print_4("jr (N)C,r8\n");
            return Operands::Registry::jr_N_C_a16;
        }
        // unconditional jr
        case 0x18: {
            debug_print_4("jr r8\n");
            return Operands::Registry::jr;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            debug_print_4("call (N)Z,a16\n");
            return Operands::Registry::call_N_Z_a16;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            debug_print_4("call (N)C,a16\n");
            return Operands::Registry::call_N_C_a16;
        }
        // call a16
        case 0xCD: {
            debug_print_4("call a16\n");
            return Operands::Registry::call_a16;
        }
        // ret (N)Z
        case 0xC0: case 0xC8: {
            debug_print_4("ret (N)Z\n");
            return Operands::Registry::ret_N_Z;
        }
        // ret (N)C
        case 0xD0: case 0xD8: {
            debug_print_4("ret (N)C\n");
            return Operands::Registry::ret_N_C;
        }
        // ret
        case 0xC9: {
            debug_print_4("ret\n");
            return Operands::Registry::ret;
        }
        // reti
        case 0xD9: {
            debug_print_4("reti\n");
            return Operands::Registry::reti;
        }
        // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            debug_print_4("rst vec\n");
            return Operands::Registry::rst_vec;
        }
        // cp r
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
            debug_print_4("cp r\n");
            return Operands::Registry::cp_r;
        }
        // cp (HL)
        case 0xBE: {
            debug_print_4("cp (HL)\n");
            return Operands::Registry::cp_HL;
        }
        // cp d8
        case 0xFE: {
            debug_print_4("cp d8\n");
            return Operands::Registry::cp_d8;
        }
        // inc ss
        case 0x03: case 0x13: case 0x23: {
            debug_print_4("inc ss\n");
            return Operands::Registry::inc_ss;
        }
        // inc SP
        case 0x33: {
            debug_print_4("inc SP\n");
            return Operands::Registry::inc_SP;
        }
        // inc (HL)
        case 0x34: {
            debug_print_4("inc (HL)\n");
            return Operands::Registry::inc_HL;
        }
        // inc r
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x3C: {
            debug_print_4("inc r\n");
            return Operands::Registry::inc_r;
        }
        // dec ss
        case 0x0B: case 0x1B: case 0x2B: {
            debug_print_4("dec ss\n");
            return Operands::Registry::dec_ss;
        }
        // dec SP
        case 0x3B: {
            debug_print_4("dec SP\n");
            return Operands::Registry::dec_SP;
        }
        // dec (HL)
        case 0x35: {
            debug_print_4("dec (HL)\n");
            return Operands::Registry::dec_HL;
        }
        // dec r
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: case 0x3D: {
            debug_print_4("dec r\n");
            return Operands::Registry::dec_r;
        }
        // or r
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: {
            debug_print_4("or r\n");
            return Operands::Registry::or_r;
        }
        // or (HL)
        case 0xB6: {
            debug_print_4("or (HL)\n");
            return Operands::Registry::or_HL;
        }
        // or d8
        case 0xF6: {
            debug_print_4("or d\n");
            return Operands::Registry::or_d8;
        }
        // and r
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: {
            debug_print_4("and r\n");
            return Operands::Registry::and_r;
        }
        // and (HL)
        case 0xA6: {
            debug_print_4("and (HL)\n");
            return Operands::Registry::and_HL;
        }
        // and d8
        case 0xE6: {
            debug_print_4("and d\n");
            return Operands::Registry::and_d8;
        }
        // xor r
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            debug_print_4("xor r\n");
            return Operands::Registry::xor_r;
        }
        // xor (HL)
        case 0xAE: {
            debug_print_4("xor (HL)\n");
            return Operands::Registry::xor_HL;
        }
        // xor d8
        case 0xEE: {
            debug_print_4("xor d\n");
            return Operands::Registry::xor_d8;
        }
        // rrca
        case 0x0F: {
            debug_print_4("rrca\n");
            return Operands::Registry::rrca;
        }
        // rlca
        case 0x07: {
            debug_print_4("rlca\n");
            return Operands::Registry::rlca;
        }
        // rra
        case 0x1F: {
            debug_print_4("rra\n");
            return Operands::Registry::rra;
        }
        // rla
        case 0x17: {
            debug_print_4("rla\n");
            return Operands::Registry::rla;
        }
        // pop rr
        case 0xC1: case 0xD1: case 0xE1: {
            debug_print_4("pop rr\n");
            return Operands::Registry::pop_rr;
        }
        // pop AF
        case 0xF1: {
            debug_print_4("pop AF\n");
            return Operands::Registry::pop_AF;
        }
        // push rr
        case 0xC5: case 0xD5: case 0xE5: {
            debug_print_4("push rr\n");
            return Operands::Registry::push_rr;
        }
        // push AF
        case 0xF5: {
            debug_print_4("push AF\n");
            return Operands::Registry::push_AF;
        }
        // daa
        case 0x27: {
            debug_print_4("daa\n");
            return Operands::Registry::daa;
        }
        // cpl
        case 0x2F: {
            debug_print_4("cpl\n");
            return Operands::Registry::cpl;
        }
        // scf
        case 0x37: {
            debug_print_4("scf\n");
            return Operands::Registry::scf;
        }
        // ccf
        case 0x3F: {
            debug_print_4("ccf\n");
            return Operands::Registry::ccf;
        }
        // di
        case 0xF3: {
            debug_print_4("di\n");
            return Operands::Registry::di;
        }
        // ei
        case 0xFB: {
            debug_print_4("ei\n");
            return Operands::Registry::ei;
        }
        // stop
        case 0x10: {
            debug_print_4("stop\n");
            return Operands::Registry::stop;
        }
        // halt
        case 0x76: {
            debug_print_4("halt\n");
            return Operands::Registry::halt;
        }
        // prefix
        case 0xCB: {
            debug_print_4("prefix (CB)\n");
            operands[0] = Operands::decodePrefix;
            // Small workaround:
            // To avoid our fetch/execution overlap implementation to override the prefix operands fetched in
            // Operands::decodePrefix, we add a dummy operand. Like this when we check if the next operand would
            // have been null, we do not directly replace the prefix operands. We only do it once they have been
            // executed.
            operands[1] = Operands::nop;
            operands[2] = nullptr;
            return true;
        }
        default: {
            return nullptr;
        }
    }
}
