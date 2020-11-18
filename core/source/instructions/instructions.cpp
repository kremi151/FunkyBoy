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
        default:
            return 0;
    }
}
