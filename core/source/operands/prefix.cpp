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

#include <util/debug.h>
#include <util/flags.h>

#include <operands/reads.h>
#include <operands/writes.h>
#include <operands/misc.h>
#include <operands/debug.h>
#include <operands/tables.h>

using namespace FunkyBoy;

bool __prefix__rlc_r(InstrContext &context, Memory &memory) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg << 1) | ((*reg >> 7) & 0b1);
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
    *reg = newVal;
    return true;
}

bool __prefix__rlc_lsb(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal << 1) | ((oldVal >> 7) & 0b1);
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix__rrc_r(InstrContext &context, Memory &memory) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg >> 1) | ((*reg & 0b1) << 7);
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b1) > 0);
    *reg = newVal;
    return true;
}

bool __prefix__rrc_lsb(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal >> 1) | ((oldVal & 0b1) << 7);
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b1) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix_rl_r(InstrContext &context, Memory &memory) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg << 1);
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b1;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
    *reg = newVal;
    return true;
}

bool __prefix_rl_HL(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal << 1);
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b1;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix_rr_r(InstrContext &context, Memory &memory) {
    // 0x18 -> 11 000 -> B
    // 0x19 -> 11 001 -> C
    // 0x1A -> 11 010 -> D
    // 0x1B -> 11 011 -> E
    // 0x1C -> 11 100 -> H
    // 0x1D -> 11 101 -> L
    // --- Skip F ---
    // 0x1F -> 11 111 -> A
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = *reg >> 1;
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b10000000;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, *reg & 0b1);
    *reg = newVal;
    return true;
}

bool __prefix_rr_HL(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = oldVal >> 1;
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b10000000;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, oldVal & 0b1);
    context.lsb = newVal;
    return true;
}

bool __prefix_sla_r(InstrContext &context, Memory &memory) {
    // 0x20 -> 100 000 -> B
    // 0x21 -> 100 001 -> C
    // 0x22 -> 100 010 -> D
    // 0x23 -> 100 011 -> E
    // 0x24 -> 100 100 -> H
    // 0x25 -> 100 101 -> L
    // --- Skip F ---
    // 0x27 -> 100 111 -> A
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = *reg << 1;
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
    *reg = newVal;
    return true;
}

bool __prefix_sla_HL(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = oldVal << 1;
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix_sra_r(InstrContext &context, Memory &memory) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg >> 1) | (*reg & 0b10000000);
    Flags::setFlags(context.regF, newVal == 0, false, false, *reg & 0b1);
    *reg = newVal;
    return true;
}

bool __prefix_sra_HL(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal >> 1) | (oldVal & 0b10000000);
    Flags::setFlags(context.regF, newVal == 0, false, false, oldVal & 0b1);
    context.lsb = newVal;
    return true;
}

bool __prefix_swap_r(InstrContext &context, Memory &memory) {
    u8 *reg = context.registers + (context.instr & 0b111);
    *reg = ((*reg >> 4) & 0b1111) | ((*reg & 0b1111) << 4);
    Flags::setFlags(context.regF, *reg == 0, false, false, false);
    return true;
}

bool __prefix_swap_HL(InstrContext &context, Memory &memory) {
    u8 val = context.lsb;
    val = ((val >> 4) & 0b1111) | ((val & 0b1111) << 4);
    context.lsb = val;
    Flags::setFlags(context.regF, val == 0, false, false, false);
    return true;
}

bool __prefix_srl_r(InstrContext &context, Memory &memory) {
    // 0x38 -> 111 000 -> B
    // 0x39 -> 111 001 -> C
    // 0x3A -> 111 010 -> D
    // 0x3B -> 111 011 -> E
    // 0x3C -> 111 100 -> H
    // 0x3D -> 111 101 -> L
    // --- Skip F ---
    // 0x3F -> 111 111 -> A
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = *reg >> 1;
    Flags::setFlags(context.regF, newVal == 0, false, false, *reg & 0b1);
    *reg = newVal;
    return true;
}

bool __prefix_srl_HL(InstrContext &context, Memory &memory) {
    u8 oldVal = context.lsb;
    u8 newVal = oldVal >> 1;
    Flags::setFlags(context.regF, newVal == 0, false, false, oldVal & 0b1);
    context.lsb = newVal;
    return true;
}

bool __prefix_bit_r(InstrContext &context, Memory &memory) {
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

    u8 bitShift = (context.instr >> 3) & 0b111;
    u8 bitMask = 1 << bitShift;
    u8 regPos = context.instr & 0b111;
    u8 *reg = context.registers + regPos;
    // Note: We write the opposite of the Nth bit into the Z flag
    Flags::setFlags(context.regF, !(*reg & bitMask), false, true, Flags::isCarry(context.regF));
    return true;
}

bool __prefix_bit_HL(InstrContext &context, Memory &memory) {
    // 0x46 -> 1 000 110 -> 0
    // 0x4E -> 1 001 110 -> 1
    // 0x56 -> 1 010 110 -> 2
    // 0x5E -> 1 011 110 -> 3
    // 0x66 -> 1 100 110 -> 4
    // 0x6E -> 1 101 110 -> 5
    // 0x76 -> 1 110 110 -> 6
    // 0x7E -> 1 111 110 -> 6
    u8 bitShift = (context.instr >> 3) & 0b111;
    u8 bitMask = 1 << bitShift;
    // Note: We write the opposite of the Nth bit into the Z flag
    Flags::setFlags(context.regF, !(context.lsb & bitMask), false, true, Flags::isCarry(context.regF));
    return true;
}

bool __prefix_res_r(InstrContext &context, Memory &memory) {
    u8 bitShift = (context.instr >> 3) & 0b111;
    u8 regPos = context.instr & 0b111;
    u8 *reg = context.registers + regPos;
    *reg &= ~(1 << bitShift);
    return true;
}

bool __prefix_res_HL(InstrContext &context, Memory &memory) {
    u8 bitShift = (context.instr >> 3) & 0b111;
    u8 val = context.lsb;
    val &= ~(1 << bitShift);
    context.lsb = val;
    return true;
}

bool __prefix_set_r(InstrContext &context, Memory &memory) {
    u8 bitMask = (context.instr >> 3) & 0b111;
    u8 regPos = context.instr & 0b111;
    u8 *reg = context.registers + regPos;
    *reg |= (1 << bitMask);
    return true;
}

bool __prefix_set_HL(InstrContext &context, Memory &memory) {
    u8 bitMask = (context.instr >> 3) & 0b111;
    u8 val = context.lsb;
    val |= (1 << bitMask);
    context.lsb = val;
    return true;
}

namespace FunkyBoy::Operands::Registry {

    const Operand rlc_r[3] = {
            Operands::nop,
            __prefix__rlc_r,
            nullptr
    };
    const Operand rlc_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix__rlc_lsb,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand rrc_r[3] = {
            Operands::nop,
            __prefix__rrc_r,
            nullptr
    };
    const Operand rrc_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix__rrc_lsb,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand rl_r[3] = {
            Operands::nop,
            __prefix_rl_r,
            nullptr
    };
    const Operand rl_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_rl_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand rr_r[3] = {
            Operands::nop,
            __prefix_rr_r,
            nullptr
    };
    const Operand rr_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_rr_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand sla_r[3] = {
            Operands::nop,
            __prefix_sla_r,
            nullptr
    };
    const Operand sla_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_sla_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand sra_r[3] = {
            Operands::nop,
            __prefix_sra_r,
            nullptr
    };
    const Operand sra_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_sra_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand swap_r[3] = {
            Operands::nop,
            __prefix_swap_r,
            nullptr
    };
    const Operand swap_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_swap_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand srl_r[3] = {
            Operands::nop,
            __prefix_srl_r,
            nullptr
    };
    const Operand srl_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_srl_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand bit_n_r[3] = {
            Operands::nop,
            __prefix_bit_r,
            nullptr
    };
    const Operand bit_n_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_bit_HL,
            Operands::_pad_,
            nullptr
    };

    const Operand res_n_r[3] = {
            Operands::nop,
            __prefix_res_r,
            nullptr
    };
    const Operand res_n_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_res_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

    const Operand set_n_r[3] = {
            Operands::nop,
            __prefix_set_r,
            nullptr
    };
    const Operand set_n_HL[5] = {
            Operands::nop,
            Operands::readHLMem,
            __prefix_set_HL,
            Operands::writeLSBIntoHLMem,
            nullptr
    };

}

namespace FunkyBoy::Operands::Tables {

    Operand const* const prefixInstructions[256] = {
            Registry::rlc_r,        // 0x00 RLC B
            Registry::rlc_r,        // 0x01 RLC C
            Registry::rlc_r,        // 0x02 RLC D
            Registry::rlc_r,        // 0x03 RLC E
            Registry::rlc_r,        // 0x04 RLC H
            Registry::rlc_r,        // 0x05 RLC L
            Registry::rlc_HL,       // 0x06 RLC (HL)
            Registry::rlc_r,        // 0x07 RLC A
            Registry::rrc_r,        // 0x08 RRC B
            Registry::rrc_r,        // 0x09 RRC C
            Registry::rrc_r,        // 0x0A RRC D
            Registry::rrc_r,        // 0x0B RRC E
            Registry::rrc_r,        // 0x0C RRC H
            Registry::rrc_r,        // 0x0D RRC L
            Registry::rrc_HL,       // 0x0E RRC (HL)
            Registry::rrc_r,        // 0x0F RRC A
            Registry::rl_r, // 0x10 RL B
            Registry::rl_r, // 0x11 RL C
            Registry::rl_r, // 0x12 RL D
            Registry::rl_r, // 0x13 RL E
            Registry::rl_r, // 0x14 RL H
            Registry::rl_r, // 0x15 RL L
            Registry::rl_HL,        // 0x16 RL (HL)
            Registry::rl_r, // 0x17 RL A
            Registry::rr_r, // 0x18 RR B
            Registry::rr_r, // 0x19 RR C
            Registry::rr_r, // 0x1A RR D
            Registry::rr_r, // 0x1B RR E
            Registry::rr_r, // 0x1C RR H
            Registry::rr_r, // 0x1D RR L
            Registry::rr_HL,        // 0x1E RR (HL)
            Registry::rr_r, // 0x1F RR A
            Registry::sla_r,        // 0x20 SLA B
            Registry::sla_r,        // 0x21 SLA C
            Registry::sla_r,        // 0x22 SLA D
            Registry::sla_r,        // 0x23 SLA E
            Registry::sla_r,        // 0x24 SLA H
            Registry::sla_r,        // 0x25 SLA L
            Registry::sla_HL,       // 0x26 SLA (HL)
            Registry::sla_r,        // 0x27 SLA A
            Registry::sra_r,        // 0x28 SRA B
            Registry::sra_r,        // 0x29 SRA C
            Registry::sra_r,        // 0x2A SRA D
            Registry::sra_r,        // 0x2B SRA E
            Registry::sra_r,        // 0x2C SRA H
            Registry::sra_r,        // 0x2D SRA L
            Registry::sra_HL,       // 0x2E SRA (HL)
            Registry::sra_r,        // 0x2F SRA A
            Registry::swap_r,       // 0x30 SWAP B
            Registry::swap_r,       // 0x31 SWAP C
            Registry::swap_r,       // 0x32 SWAP D
            Registry::swap_r,       // 0x33 SWAP E
            Registry::swap_r,       // 0x34 SWAP H
            Registry::swap_r,       // 0x35 SWAP L
            Registry::swap_HL,      // 0x36 SWAP (HL)
            Registry::swap_r,       // 0x37 SWAP A
            Registry::srl_r,        // 0x38 SRL B
            Registry::srl_r,        // 0x39 SRL C
            Registry::srl_r,        // 0x3A SRL D
            Registry::srl_r,        // 0x3B SRL E
            Registry::srl_r,        // 0x3C SRL H
            Registry::srl_r,        // 0x3D SRL L
            Registry::srl_HL,       // 0x3E SRL (HL)
            Registry::srl_r,        // 0x3F SRL A
            Registry::bit_n_r,      // 0x40 BIT 0,B
            Registry::bit_n_r,      // 0x41 BIT 0,C
            Registry::bit_n_r,      // 0x42 BIT 0,D
            Registry::bit_n_r,      // 0x43 BIT 0,E
            Registry::bit_n_r,      // 0x44 BIT 0,H
            Registry::bit_n_r,      // 0x45 BIT 0,L
            Registry::bit_n_HL,     // 0x46 BIT 0,(HL)
            Registry::bit_n_r,      // 0x47 BIT 0,A
            Registry::bit_n_r,      // 0x48 BIT 1,B
            Registry::bit_n_r,      // 0x49 BIT 1,C
            Registry::bit_n_r,      // 0x4A BIT 1,D
            Registry::bit_n_r,      // 0x4B BIT 1,E
            Registry::bit_n_r,      // 0x4C BIT 1,H
            Registry::bit_n_r,      // 0x4D BIT 1,L
            Registry::bit_n_HL,     // 0x4E BIT 1,(HL)
            Registry::bit_n_r,      // 0x4F BIT 1,A
            Registry::bit_n_r,      // 0x50 BIT 2,B
            Registry::bit_n_r,      // 0x51 BIT 2,C
            Registry::bit_n_r,      // 0x52 BIT 2,D
            Registry::bit_n_r,      // 0x53 BIT 2,E
            Registry::bit_n_r,      // 0x54 BIT 2,H
            Registry::bit_n_r,      // 0x55 BIT 2,L
            Registry::bit_n_HL,     // 0x56 BIT 2,(HL)
            Registry::bit_n_r,      // 0x57 BIT 2,A
            Registry::bit_n_r,      // 0x58 BIT 3,B
            Registry::bit_n_r,      // 0x59 BIT 3,C
            Registry::bit_n_r,      // 0x5A BIT 3,D
            Registry::bit_n_r,      // 0x5B BIT 3,E
            Registry::bit_n_r,      // 0x5C BIT 3,H
            Registry::bit_n_r,      // 0x5D BIT 3,L
            Registry::bit_n_HL,     // 0x5E BIT 3,(HL)
            Registry::bit_n_r,      // 0x5F BIT 3,A
            Registry::bit_n_r,      // 0x60 BIT 4,B
            Registry::bit_n_r,      // 0x61 BIT 4,C
            Registry::bit_n_r,      // 0x62 BIT 4,D
            Registry::bit_n_r,      // 0x63 BIT 4,E
            Registry::bit_n_r,      // 0x64 BIT 4,H
            Registry::bit_n_r,      // 0x65 BIT 4,L
            Registry::bit_n_HL,     // 0x66 BIT 4,(HL)
            Registry::bit_n_r,      // 0x67 BIT 4,A
            Registry::bit_n_r,      // 0x68 BIT 5,B
            Registry::bit_n_r,      // 0x69 BIT 5,C
            Registry::bit_n_r,      // 0x6A BIT 5,D
            Registry::bit_n_r,      // 0x6B BIT 5,E
            Registry::bit_n_r,      // 0x6C BIT 5,H
            Registry::bit_n_r,      // 0x6D BIT 5,L
            Registry::bit_n_HL,     // 0x6E BIT 5,(HL)
            Registry::bit_n_r,      // 0x6F BIT 5,A
            Registry::bit_n_r,      // 0x70 BIT 6,B
            Registry::bit_n_r,      // 0x71 BIT 6,C
            Registry::bit_n_r,      // 0x72 BIT 6,D
            Registry::bit_n_r,      // 0x73 BIT 6,E
            Registry::bit_n_r,      // 0x74 BIT 6,H
            Registry::bit_n_r,      // 0x75 BIT 6,L
            Registry::bit_n_HL,     // 0x76 BIT 6,(HL)
            Registry::bit_n_r,      // 0x77 BIT 6,A
            Registry::bit_n_r,      // 0x78 BIT 7,B
            Registry::bit_n_r,      // 0x79 BIT 7,C
            Registry::bit_n_r,      // 0x7A BIT 7,D
            Registry::bit_n_r,      // 0x7B BIT 7,E
            Registry::bit_n_r,      // 0x7C BIT 7,H
            Registry::bit_n_r,      // 0x7D BIT 7,L
            Registry::bit_n_HL,     // 0x7E BIT 7,(HL)
            Registry::bit_n_r,      // 0x7F BIT 7,A
            Registry::res_n_r,      // 0x80 RES 0,B
            Registry::res_n_r,      // 0x81 RES 0,C
            Registry::res_n_r,      // 0x82 RES 0,D
            Registry::res_n_r,      // 0x83 RES 0,E
            Registry::res_n_r,      // 0x84 RES 0,H
            Registry::res_n_r,      // 0x85 RES 0,L
            Registry::res_n_HL,     // 0x86 RES 0,(HL)
            Registry::res_n_r,      // 0x87 RES 0,A
            Registry::res_n_r,      // 0x88 RES 1,B
            Registry::res_n_r,      // 0x89 RES 1,C
            Registry::res_n_r,      // 0x8A RES 1,D
            Registry::res_n_r,      // 0x8B RES 1,E
            Registry::res_n_r,      // 0x8C RES 1,H
            Registry::res_n_r,      // 0x8D RES 1,L
            Registry::res_n_HL,     // 0x8E RES 1,(HL)
            Registry::res_n_r,      // 0x8F RES 1,A
            Registry::res_n_r,      // 0x90 RES 2,B
            Registry::res_n_r,      // 0x91 RES 2,C
            Registry::res_n_r,      // 0x92 RES 2,D
            Registry::res_n_r,      // 0x93 RES 2,E
            Registry::res_n_r,      // 0x94 RES 2,H
            Registry::res_n_r,      // 0x95 RES 2,L
            Registry::res_n_HL,     // 0x96 RES 2,(HL)
            Registry::res_n_r,      // 0x97 RES 2,A
            Registry::res_n_r,      // 0x98 RES 3,B
            Registry::res_n_r,      // 0x99 RES 3,C
            Registry::res_n_r,      // 0x9A RES 3,D
            Registry::res_n_r,      // 0x9B RES 3,E
            Registry::res_n_r,      // 0x9C RES 3,H
            Registry::res_n_r,      // 0x9D RES 3,L
            Registry::res_n_HL,     // 0x9E RES 3,(HL)
            Registry::res_n_r,      // 0x9F RES 3,A
            Registry::res_n_r,      // 0xA0 RES 4,B
            Registry::res_n_r,      // 0xA1 RES 4,C
            Registry::res_n_r,      // 0xA2 RES 4,D
            Registry::res_n_r,      // 0xA3 RES 4,E
            Registry::res_n_r,      // 0xA4 RES 4,H
            Registry::res_n_r,      // 0xA5 RES 4,L
            Registry::res_n_HL,     // 0xA6 RES 4,(HL)
            Registry::res_n_r,      // 0xA7 RES 4,A
            Registry::res_n_r,      // 0xA8 RES 5,B
            Registry::res_n_r,      // 0xA9 RES 5,C
            Registry::res_n_r,      // 0xAA RES 5,D
            Registry::res_n_r,      // 0xAB RES 5,E
            Registry::res_n_r,      // 0xAC RES 5,H
            Registry::res_n_r,      // 0xAD RES 5,L
            Registry::res_n_HL,     // 0xAE RES 5,(HL)
            Registry::res_n_r,      // 0xAF RES 5,A
            Registry::res_n_r,      // 0xB0 RES 6,B
            Registry::res_n_r,      // 0xB1 RES 6,C
            Registry::res_n_r,      // 0xB2 RES 6,D
            Registry::res_n_r,      // 0xB3 RES 6,E
            Registry::res_n_r,      // 0xB4 RES 6,H
            Registry::res_n_r,      // 0xB5 RES 6,L
            Registry::res_n_HL,     // 0xB6 RES 6,(HL)
            Registry::res_n_r,      // 0xB7 RES 6,A
            Registry::res_n_r,      // 0xB8 RES 7,B
            Registry::res_n_r,      // 0xB9 RES 7,C
            Registry::res_n_r,      // 0xBA RES 7,D
            Registry::res_n_r,      // 0xBB RES 7,E
            Registry::res_n_r,      // 0xBC RES 7,H
            Registry::res_n_r,      // 0xBD RES 7,L
            Registry::res_n_HL,     // 0xBE RES 7,(HL)
            Registry::res_n_r,      // 0xBF RES 7,A
            Registry::set_n_r,      // 0xC0 SET 0,B
            Registry::set_n_r,      // 0xC1 SET 0,C
            Registry::set_n_r,      // 0xC2 SET 0,D
            Registry::set_n_r,      // 0xC3 SET 0,E
            Registry::set_n_r,      // 0xC4 SET 0,H
            Registry::set_n_r,      // 0xC5 SET 0,L
            Registry::set_n_HL,     // 0xC6 SET 0,(HL)
            Registry::set_n_r,      // 0xC7 SET 0,A
            Registry::set_n_r,      // 0xC8 SET 1,B
            Registry::set_n_r,      // 0xC9 SET 1,C
            Registry::set_n_r,      // 0xCA SET 1,D
            Registry::set_n_r,      // 0xCB SET 1,E
            Registry::set_n_r,      // 0xCC SET 1,H
            Registry::set_n_r,      // 0xCD SET 1,L
            Registry::set_n_HL,     // 0xCE SET 1,(HL)
            Registry::set_n_r,      // 0xCF SET 1,A
            Registry::set_n_r,      // 0xD0 SET 2,B
            Registry::set_n_r,      // 0xD1 SET 2,C
            Registry::set_n_r,      // 0xD2 SET 2,D
            Registry::set_n_r,      // 0xD3 SET 2,E
            Registry::set_n_r,      // 0xD4 SET 2,H
            Registry::set_n_r,      // 0xD5 SET 2,L
            Registry::set_n_HL,     // 0xD6 SET 2,(HL)
            Registry::set_n_r,      // 0xD7 SET 2,A
            Registry::set_n_r,      // 0xD8 SET 3,B
            Registry::set_n_r,      // 0xD9 SET 3,C
            Registry::set_n_r,      // 0xDA SET 3,D
            Registry::set_n_r,      // 0xDB SET 3,E
            Registry::set_n_r,      // 0xDC SET 3,H
            Registry::set_n_r,      // 0xDD SET 3,L
            Registry::set_n_HL,     // 0xDE SET 3,(HL)
            Registry::set_n_r,      // 0xDF SET 3,A
            Registry::set_n_r,      // 0xE0 SET 4,B
            Registry::set_n_r,      // 0xE1 SET 4,C
            Registry::set_n_r,      // 0xE2 SET 4,D
            Registry::set_n_r,      // 0xE3 SET 4,E
            Registry::set_n_r,      // 0xE4 SET 4,H
            Registry::set_n_r,      // 0xE5 SET 4,L
            Registry::set_n_HL,     // 0xE6 SET 4,(HL)
            Registry::set_n_r,      // 0xE7 SET 4,A
            Registry::set_n_r,      // 0xE8 SET 5,B
            Registry::set_n_r,      // 0xE9 SET 5,C
            Registry::set_n_r,      // 0xEA SET 5,D
            Registry::set_n_r,      // 0xEB SET 5,E
            Registry::set_n_r,      // 0xEC SET 5,H
            Registry::set_n_r,      // 0xED SET 5,L
            Registry::set_n_HL,     // 0xEE SET 5,(HL)
            Registry::set_n_r,      // 0xEF SET 5,A
            Registry::set_n_r,      // 0xF0 SET 6,B
            Registry::set_n_r,      // 0xF1 SET 6,C
            Registry::set_n_r,      // 0xF2 SET 6,D
            Registry::set_n_r,      // 0xF3 SET 6,E
            Registry::set_n_r,      // 0xF4 SET 6,H
            Registry::set_n_r,      // 0xF5 SET 6,L
            Registry::set_n_HL,     // 0xF6 SET 6,(HL)
            Registry::set_n_r,      // 0xF7 SET 6,A
            Registry::set_n_r,      // 0xF8 SET 7,B
            Registry::set_n_r,      // 0xF9 SET 7,C
            Registry::set_n_r,      // 0xFA SET 7,D
            Registry::set_n_r,      // 0xFB SET 7,E
            Registry::set_n_r,      // 0xFC SET 7,H
            Registry::set_n_r,      // 0xFD SET 7,L
            Registry::set_n_HL,     // 0xFE SET 7,(HL)
            Registry::set_n_r,      // 0xFF SET 7,A
    };

}

bool Operands::decodePrefix(InstrContext &context, Memory &memory) {
#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    FunkyBoy::Debug::writeExecutionToLog('P', *context.executionLog, context);
#endif

    context.instr = memory.read8BitsAt(context.progCounter++);
    *context.operandsPtr = Tables::prefixInstructions[context.instr];
    return true;
}
