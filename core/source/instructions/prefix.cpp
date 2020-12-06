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

#include "prefix.h"

#include <util/flags.h>
#include <util/debug.h>

using namespace FunkyBoy::Instructions;

namespace FunkyBoy::Instructions::Prefix {

    void rlc_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = ((*reg << 1) | ((*reg >> 7) & 0b1)) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (*reg & 0b10000000u) > 0);
        *reg = newVal;
    }

    void rlc_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = ((oldVal << 1) | ((oldVal >> 7) & 0b1)) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (oldVal & 0b10000000u) > 0);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void rrc_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = ((*reg & 0xffu) >> 1) | ((*reg & 0b1) << 7);
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (*reg & 0b1u) > 0);
        *reg = newVal;
    }

    void rrc_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = ((oldVal & 0xffu) >> 1) | ((oldVal & 0b1u) << 7);
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (oldVal & 0b1u) > 0);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void rl_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = (*reg << 1) & 0xffu;
        if (Flags::isCarryFast(*context.regF)) {
            newVal |= 0b1u;
        }
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (*reg & 0b10000000u) > 0);
        *reg = newVal;
    }

    void rl_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = (oldVal << 1) & 0xffu;
        if (Flags::isCarryFast(*context.regF)) {
            newVal |= 0b1u;
        }
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (oldVal & 0b10000000u) > 0);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void rr_r(opcode_t opcode, context &context, Memory &memory) {
        // 0x18 -> 11 000 -> B
        // 0x19 -> 11 001 -> C
        // 0x1A -> 11 010 -> D
        // 0x1B -> 11 011 -> E
        // 0x1C -> 11 100 -> H
        // 0x1D -> 11 101 -> L
        // --- Skip F ---
        // 0x1F -> 11 111 -> A
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = (*reg & 0xffu) >> 1;
        if (Flags::isCarryFast(*context.regF)) {
            newVal |= 0b10000000u;
        }
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, *reg & 0b1u);
        *reg = newVal;
    }

    void rr_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = (oldVal & 0xffu) >> 1;
        if (Flags::isCarryFast(*context.regF)) {
            newVal |= 0b10000000u;
        }
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, oldVal & 0b1u);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void sla_r(opcode_t opcode, context &context, Memory &memory) {
        // 0x20 -> 100 000 -> B
        // 0x21 -> 100 001 -> C
        // 0x22 -> 100 010 -> D
        // 0x23 -> 100 011 -> E
        // 0x24 -> 100 100 -> H
        // 0x25 -> 100 101 -> L
        // --- Skip F ---
        // 0x27 -> 100 111 -> A
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = (*reg << 1) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (*reg & 0b10000000u) > 0);
        *reg = newVal;
    }

    void sla_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = (oldVal << 1) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (oldVal & 0b10000000u) > 0);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void sra_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = ((*reg & 0xffu) >> 1) | (*reg & 0b10000000);
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, *reg & 0b1u);
        *reg = newVal;
    }

    void sra_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL()) & 0xffu;
        u8_fast newVal = (oldVal >> 1) | (oldVal & 0b10000000);
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, oldVal & 0b1u);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void swap_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast &reg = *(context.registers + (opcode & 0b111u));
        reg = ((reg >> 4) & 0b1111u) | ((reg & 0b1111u) << 4);
        Flags::setFlagsFast(*context.regF, reg == 0, false, false, false);
    }

    void swap_HL(context &context, Memory &memory) {
        u8_fast val = memory.read8BitsAt(context.readHL());
        val = ((val >> 4) & 0b1111u) | ((val & 0b1111u) << 4);
        memory.write8BitsTo(context.readHL(), val);
        Flags::setFlagsFast(*context.regF, val == 0, false, false, false);
    }

    void srl_r(opcode_t opcode, context &context, Memory &memory) {
        // 0x38 -> 111 000 -> B
        // 0x39 -> 111 001 -> C
        // 0x3A -> 111 010 -> D
        // 0x3B -> 111 011 -> E
        // 0x3C -> 111 100 -> H
        // 0x3D -> 111 101 -> L
        // --- Skip F ---
        // 0x3F -> 111 111 -> A
        u8_fast *reg = context.registers + (opcode & 0b111u);
        u8_fast newVal = (*reg & 0xffu) >> 1;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, *reg & 0b1u);
        *reg = newVal;
    }

    void srl_HL(context &context, Memory &memory) {
        u8_fast oldVal = memory.read8BitsAt(context.readHL());
        u8_fast newVal = (oldVal >> 1) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, oldVal & 0b1);
        memory.write8BitsTo(context.readHL(), newVal);
    }

    void bit_r(opcode_t opcode, context &context, Memory &memory) {
        // 0x40 -> 1 000 000 -> 0,B
        // 0x41 -> 1 000 001 -> 0,C
        // 0x42 -> 1 000 010 -> 0,D
        // 0x43 -> 1 000 011 -> 0,E
        // 0x44 -> 1 000 100 -> 0,H
        // 0x45 -> 1 000 101 -> 0,L
        // --- Skip F ---
        // 0x47 -> 1 000 111 -> 0,A

        // 0x48 -> 1 001 000 -> 1,B
        // 0x49 -> 1 001 001 -> 1,C
        // 0x4A -> 1 001 010 -> 1,D
        // 0x4B -> 1 001 011 -> 1,E
        // 0x4C -> 1 001 100 -> 1,H
        // 0x4D -> 1 001 101 -> 1,L
        // --- Skip F ---
        // 0x4F -> 1 001 111 -> 1,A

        // 0x50 -> 1 010 000 -> 2,B
        // 0x51 -> 1 010 001 -> 2,C
        // 0x52 -> 1 010 010 -> 2,D
        // 0x53 -> 1 010 011 -> 2,E
        // 0x54 -> 1 010 100 -> 2,H
        // 0x55 -> 1 010 101 -> 2,L
        // --- Skip F ---
        // 0x57 -> 1 010 111 -> 2,A

        // etc...

        u8_fast bitShift = (opcode >> 3) & 0b111u;
        u8_fast bitMask = 1u << bitShift;
        u8_fast regPos = opcode & 0b111u;
        // Note: We write the opposite of the Nth bit into the Z flag
        Flags::setFlagsFast(*context.regF, !(*(context.registers + regPos) & bitMask), false, true, Flags::isCarryFast(*context.regF));
    }

    void bit_HL(opcode_t opcode, context &context, Memory &memory) {
        // 0x46 -> 1 000 110 -> 0
        // 0x4E -> 1 001 110 -> 1
        // 0x56 -> 1 010 110 -> 2
        // 0x5E -> 1 011 110 -> 3
        // 0x66 -> 1 100 110 -> 4
        // 0x6E -> 1 101 110 -> 5
        // 0x76 -> 1 110 110 -> 6
        // 0x7E -> 1 111 110 -> 6
        u8_fast bitShift = (opcode >> 3) & 0b111u;
        u8_fast bitMask = 1u << bitShift;
        // Note: We write the opposite of the Nth bit into the Z flag
        Flags::setFlagsFast(*context.regF, !(memory.read8BitsAt(context.readHL()) & bitMask), false, true, Flags::isCarryFast(*context.regF));
    }

    void res_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast bitShift = (opcode >> 3) & 0b111u;
        u8_fast regPos = opcode & 0b111u;
        *(context.registers + regPos) &= ~(1u << bitShift);
    }

    void res_HL(opcode_t opcode, context &context, Memory &memory) {
        u8_fast bitShift = (opcode >> 3) & 0b111u;
        u8_fast val = memory.read8BitsAt(context.readHL()) & ~(1u << bitShift);
        memory.write8BitsTo(context.readHL(), val);
    }

    void set_r(opcode_t opcode, context &context, Memory &memory) {
        u8_fast bitMask = (opcode >> 3) & 0b111u;
        u8_fast regPos = opcode & 0b111u;
        *(context.registers + regPos) |= (1u << bitMask);
    }

    void set_HL(opcode_t opcode, context &context, Memory &memory) {
        u8_fast bitMask = (opcode >> 3) & 0b111u;
        u8_fast val = memory.read8BitsAt(context.readHL()) | (1u << bitMask);
        memory.write8BitsTo(context.readHL(), val);
    }

}

int Prefix::execute(opcode_t opcode, context &context, Memory &memory) {
    switch (opcode) {
        /* rlc reg */ case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
            debug_print_4("rlc r\n");
            Prefix::rlc_r(opcode, context, memory);
            return 8;
        }
        /* rlc (HL) */ case 0x06: {
            debug_print_4("rlc (HL)\n");
            Prefix::rlc_HL(context, memory);
            return 16;
        }
        /* rrc reg */ case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: {
            debug_print_4("rrc r\n");
            Prefix::rrc_r(opcode, context, memory);
            return 8;
        }
        /* rrc (HL) */ case 0x0E: {
            debug_print_4("rrc (HL)\n");
            Prefix::rrc_HL(context, memory);
            return 16;
        }
        /* rl reg */ case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: {
            debug_print_4("rl r\n");
            Prefix::rl_r(opcode, context, memory);
            return 8;
        }
        /* rl (HL) */ case 0x16: {
            debug_print_4("rl (HL)\n");
            Prefix::rl_HL(context, memory);
            return 16;
        }
        /* rr reg */ case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1F: {
            debug_print_4("rr r\n");
            Prefix::rr_r(opcode, context, memory);
            return 8;
        }
        /* rr (HL) */ case 0x1E: {
            debug_print_4("rr (HL)\n");
            Prefix::rr_HL(context, memory);
            return 16;
        }
        /* sla reg */ case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27: {
            debug_print_4("sla r\n");
            Prefix::sla_r(opcode, context, memory);
            return 8;
        }
        /* sla (HL) */ case 0x26: {
            debug_print_4("sla (HL)\n");
            Prefix::sla_HL(context, memory);
            return 16;
        }
        /* sra reg */ case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F: {
            debug_print_4("sra r\n");
            Prefix::sra_r(opcode, context, memory);
            return 8;
        }
        /* sra (HL) */ case 0x2E: {
            debug_print_4("sra (HL)\n");
            Prefix::sra_HL(context, memory);
            return 16;
        }
        /* swap reg */ case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: {
            debug_print_4("swap r\n");
            Prefix::swap_r(opcode, context, memory);
            return 8;
        }
        /* swap (HL) */ case 0x36: {
            debug_print_4("swap (HL)\n");
            Prefix::swap_HL(context, memory);
            return 16;
        }
        /* srl reg */ case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3F: {
            debug_print_4("srl r\n");
            Prefix::srl_r(opcode, context, memory);
            return 8;
        }
        /* srl (HL) */ case 0x3E: {
            debug_print_4("srl (HL)\n");
            Prefix::srl_HL(context, memory);
            return 16;
        }
        /* bit 0,reg */ case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47:
        /* bit 1,reg */ case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F:
        /* bit 2,reg */ case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57:
        /* bit 3,reg */ case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F:
        /* bit 4,reg */ case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67:
        /* bit 5,reg */ case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F:
        /* bit 6,reg */ case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
        /* bit 7,reg */ case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F:
        {
            debug_print_4("bit n,r\n");
            Prefix::bit_r(opcode, context, memory);
            return 8;
        }
        /* bit n,(HL) */ case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x76: case 0x7E: {
            debug_print_4("bit n,(HL)\n");
            Prefix::bit_HL(opcode, context, memory);
            return 16;
        }
        /* res 0,reg */ case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87:
        /* res 1,reg */ case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F:
        /* res 2,reg */ case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97:
        /* res 3,reg */ case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F:
        /* res 4,reg */ case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7:
        /* res 5,reg */ case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF:
        /* res 6,reg */ case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
        /* res 7,reg */ case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF:
        {
            debug_print_4("res r\n");
            Prefix::res_r(opcode, context, memory);
            return 8;
        }
        /* res n,(HL) */ case 0x86: case 0x8E: case 0x96: case 0x9E: case 0xA6: case 0xAE: case 0xB6: case 0xBE: {
            debug_print_4("res (HL)\n");
            Prefix::res_HL(opcode, context, memory);
            return 16;
        }
        /* set 0,reg */ case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7:
        /* set 1,reg */ case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF:
        /* set 2,reg */ case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7:
        /* set 3,reg */ case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF:
        /* set 4,reg */ case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7:
        /* set 5,reg */ case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF:
        /* set 6,reg */ case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7:
        /* set 7,reg */ case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF:
        {
            debug_print_4("set r\n");
            Prefix::set_r(opcode, context, memory);
            return 8;
        }
        /* set n,(HL) */ case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE: {
            debug_print_4("set (HL)\n");
            Prefix::set_HL(opcode, context, memory);
            return 16;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", opcode, context.progCounter - 1);
            return 0;
        }
    }
}