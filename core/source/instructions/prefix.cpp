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
        /*swap (HL) */ case 0x36: {
            debug_print_4("swap (HL)\n");
            Prefix::swap_HL(context, memory);
            return 16;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", opcode, context.progCounter - 1);
            return 0;
        }
    }
}