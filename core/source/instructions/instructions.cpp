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
        default:
            return 0;
    }
}
