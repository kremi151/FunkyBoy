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
            Registry::rlc_r,        // RLC B
            Registry::rlc_r,        // RLC C
            Registry::rlc_r,        // RLC D
            Registry::rlc_r,        // RLC E
            Registry::rlc_r,        // RLC H
            Registry::rlc_r,        // RLC L
            Registry::rlc_HL,       // RLC (HL)
            Registry::rlc_r,        // RLC A
            Registry::rrc_r,        // RRC B
            Registry::rrc_r,        // RRC C
            Registry::rrc_r,        // RRC D
            Registry::rrc_r,        // RRC E
            Registry::rrc_r,        // RRC H
            Registry::rrc_r,        // RRC L
            Registry::rrc_HL,       // RRC (HL)
            Registry::rrc_r,        // RRC A
            Registry::rl_r, // RL B
            Registry::rl_r, // RL C
            Registry::rl_r, // RL D
            Registry::rl_r, // RL E
            Registry::rl_r, // RL H
            Registry::rl_r, // RL L
            Registry::rl_HL,        // RL (HL)
            Registry::rl_r, // RL A
            Registry::rr_r, // RR B
            Registry::rr_r, // RR C
            Registry::rr_r, // RR D
            Registry::rr_r, // RR E
            Registry::rr_r, // RR H
            Registry::rr_r, // RR L
            Registry::rr_HL,        // RR (HL)
            Registry::rr_r, // RR A
            Registry::sla_r,        // SLA B
            Registry::sla_r,        // SLA C
            Registry::sla_r,        // SLA D
            Registry::sla_r,        // SLA E
            Registry::sla_r,        // SLA H
            Registry::sla_r,        // SLA L
            Registry::sla_HL,       // SLA (HL)
            Registry::sla_r,        // SLA A
            Registry::sra_r,        // SRA B
            Registry::sra_r,        // SRA C
            Registry::sra_r,        // SRA D
            Registry::sra_r,        // SRA E
            Registry::sra_r,        // SRA H
            Registry::sra_r,        // SRA L
            Registry::sra_HL,       // SRA (HL)
            Registry::sra_r,        // SRA A
            Registry::swap_r,       // SWAP B
            Registry::swap_r,       // SWAP C
            Registry::swap_r,       // SWAP D
            Registry::swap_r,       // SWAP E
            Registry::swap_r,       // SWAP H
            Registry::swap_r,       // SWAP L
            Registry::swap_HL,      // SWAP (HL)
            Registry::swap_r,       // SWAP A
            Registry::srl_r,        // SRL B
            Registry::srl_r,        // SRL C
            Registry::srl_r,        // SRL D
            Registry::srl_r,        // SRL E
            Registry::srl_r,        // SRL H
            Registry::srl_r,        // SRL L
            Registry::srl_HL,       // SRL (HL)
            Registry::srl_r,        // SRL A
            Registry::bit_n_r,      // BIT 0,B
            Registry::bit_n_r,      // BIT 0,C
            Registry::bit_n_r,      // BIT 0,D
            Registry::bit_n_r,      // BIT 0,E
            Registry::bit_n_r,      // BIT 0,H
            Registry::bit_n_r,      // BIT 0,L
            Registry::bit_n_HL,     // BIT 0,(HL)
            Registry::bit_n_r,      // BIT 0,A
            Registry::bit_n_r,      // BIT 1,B
            Registry::bit_n_r,      // BIT 1,C
            Registry::bit_n_r,      // BIT 1,D
            Registry::bit_n_r,      // BIT 1,E
            Registry::bit_n_r,      // BIT 1,H
            Registry::bit_n_r,      // BIT 1,L
            Registry::bit_n_HL,     // BIT 1,(HL)
            Registry::bit_n_r,      // BIT 1,A
            Registry::bit_n_r,      // BIT 2,B
            Registry::bit_n_r,      // BIT 2,C
            Registry::bit_n_r,      // BIT 2,D
            Registry::bit_n_r,      // BIT 2,E
            Registry::bit_n_r,      // BIT 2,H
            Registry::bit_n_r,      // BIT 2,L
            Registry::bit_n_HL,     // BIT 2,(HL)
            Registry::bit_n_r,      // BIT 2,A
            Registry::bit_n_r,      // BIT 3,B
            Registry::bit_n_r,      // BIT 3,C
            Registry::bit_n_r,      // BIT 3,D
            Registry::bit_n_r,      // BIT 3,E
            Registry::bit_n_r,      // BIT 3,H
            Registry::bit_n_r,      // BIT 3,L
            Registry::bit_n_HL,     // BIT 3,(HL)
            Registry::bit_n_r,      // BIT 3,A
            Registry::bit_n_r,      // BIT 4,B
            Registry::bit_n_r,      // BIT 4,C
            Registry::bit_n_r,      // BIT 4,D
            Registry::bit_n_r,      // BIT 4,E
            Registry::bit_n_r,      // BIT 4,H
            Registry::bit_n_r,      // BIT 4,L
            Registry::bit_n_HL,     // BIT 4,(HL)
            Registry::bit_n_r,      // BIT 4,A
            Registry::bit_n_r,      // BIT 5,B
            Registry::bit_n_r,      // BIT 5,C
            Registry::bit_n_r,      // BIT 5,D
            Registry::bit_n_r,      // BIT 5,E
            Registry::bit_n_r,      // BIT 5,H
            Registry::bit_n_r,      // BIT 5,L
            Registry::bit_n_HL,     // BIT 5,(HL)
            Registry::bit_n_r,      // BIT 5,A
            Registry::bit_n_r,      // BIT 6,B
            Registry::bit_n_r,      // BIT 6,C
            Registry::bit_n_r,      // BIT 6,D
            Registry::bit_n_r,      // BIT 6,E
            Registry::bit_n_r,      // BIT 6,H
            Registry::bit_n_r,      // BIT 6,L
            Registry::bit_n_HL,     // BIT 6,(HL)
            Registry::bit_n_r,      // BIT 6,A
            Registry::bit_n_r,      // BIT 7,B
            Registry::bit_n_r,      // BIT 7,C
            Registry::bit_n_r,      // BIT 7,D
            Registry::bit_n_r,      // BIT 7,E
            Registry::bit_n_r,      // BIT 7,H
            Registry::bit_n_r,      // BIT 7,L
            Registry::bit_n_HL,     // BIT 7,(HL)
            Registry::bit_n_r,      // BIT 7,A
            Registry::res_n_r,      // RES 0,B
            Registry::res_n_r,      // RES 0,C
            Registry::res_n_r,      // RES 0,D
            Registry::res_n_r,      // RES 0,E
            Registry::res_n_r,      // RES 0,H
            Registry::res_n_r,      // RES 0,L
            Registry::res_n_HL,     // RES 0,(HL)
            Registry::res_n_r,      // RES 0,A
            Registry::res_n_r,      // RES 1,B
            Registry::res_n_r,      // RES 1,C
            Registry::res_n_r,      // RES 1,D
            Registry::res_n_r,      // RES 1,E
            Registry::res_n_r,      // RES 1,H
            Registry::res_n_r,      // RES 1,L
            Registry::res_n_HL,     // RES 1,(HL)
            Registry::res_n_r,      // RES 1,A
            Registry::res_n_r,      // RES 2,B
            Registry::res_n_r,      // RES 2,C
            Registry::res_n_r,      // RES 2,D
            Registry::res_n_r,      // RES 2,E
            Registry::res_n_r,      // RES 2,H
            Registry::res_n_r,      // RES 2,L
            Registry::res_n_HL,     // RES 2,(HL)
            Registry::res_n_r,      // RES 2,A
            Registry::res_n_r,      // RES 3,B
            Registry::res_n_r,      // RES 3,C
            Registry::res_n_r,      // RES 3,D
            Registry::res_n_r,      // RES 3,E
            Registry::res_n_r,      // RES 3,H
            Registry::res_n_r,      // RES 3,L
            Registry::res_n_HL,     // RES 3,(HL)
            Registry::res_n_r,      // RES 3,A
            Registry::res_n_r,      // RES 4,B
            Registry::res_n_r,      // RES 4,C
            Registry::res_n_r,      // RES 4,D
            Registry::res_n_r,      // RES 4,E
            Registry::res_n_r,      // RES 4,H
            Registry::res_n_r,      // RES 4,L
            Registry::res_n_HL,     // RES 4,(HL)
            Registry::res_n_r,      // RES 4,A
            Registry::res_n_r,      // RES 5,B
            Registry::res_n_r,      // RES 5,C
            Registry::res_n_r,      // RES 5,D
            Registry::res_n_r,      // RES 5,E
            Registry::res_n_r,      // RES 5,H
            Registry::res_n_r,      // RES 5,L
            Registry::res_n_HL,     // RES 5,(HL)
            Registry::res_n_r,      // RES 5,A
            Registry::res_n_r,      // RES 6,B
            Registry::res_n_r,      // RES 6,C
            Registry::res_n_r,      // RES 6,D
            Registry::res_n_r,      // RES 6,E
            Registry::res_n_r,      // RES 6,H
            Registry::res_n_r,      // RES 6,L
            Registry::res_n_HL,     // RES 6,(HL)
            Registry::res_n_r,      // RES 6,A
            Registry::res_n_r,      // RES 7,B
            Registry::res_n_r,      // RES 7,C
            Registry::res_n_r,      // RES 7,D
            Registry::res_n_r,      // RES 7,E
            Registry::res_n_r,      // RES 7,H
            Registry::res_n_r,      // RES 7,L
            Registry::res_n_HL,     // RES 7,(HL)
            Registry::res_n_r,      // RES 7,A
            Registry::set_n_r,      // SET 0,B
            Registry::set_n_r,      // SET 0,C
            Registry::set_n_r,      // SET 0,D
            Registry::set_n_r,      // SET 0,E
            Registry::set_n_r,      // SET 0,H
            Registry::set_n_r,      // SET 0,L
            Registry::set_n_HL,     // SET 0,(HL)
            Registry::set_n_r,      // SET 0,A
            Registry::set_n_r,      // SET 1,B
            Registry::set_n_r,      // SET 1,C
            Registry::set_n_r,      // SET 1,D
            Registry::set_n_r,      // SET 1,E
            Registry::set_n_r,      // SET 1,H
            Registry::set_n_r,      // SET 1,L
            Registry::set_n_HL,     // SET 1,(HL)
            Registry::set_n_r,      // SET 1,A
            Registry::set_n_r,      // SET 2,B
            Registry::set_n_r,      // SET 2,C
            Registry::set_n_r,      // SET 2,D
            Registry::set_n_r,      // SET 2,E
            Registry::set_n_r,      // SET 2,H
            Registry::set_n_r,      // SET 2,L
            Registry::set_n_HL,     // SET 2,(HL)
            Registry::set_n_r,      // SET 2,A
            Registry::set_n_r,      // SET 3,B
            Registry::set_n_r,      // SET 3,C
            Registry::set_n_r,      // SET 3,D
            Registry::set_n_r,      // SET 3,E
            Registry::set_n_r,      // SET 3,H
            Registry::set_n_r,      // SET 3,L
            Registry::set_n_HL,     // SET 3,(HL)
            Registry::set_n_r,      // SET 3,A
            Registry::set_n_r,      // SET 4,B
            Registry::set_n_r,      // SET 4,C
            Registry::set_n_r,      // SET 4,D
            Registry::set_n_r,      // SET 4,E
            Registry::set_n_r,      // SET 4,H
            Registry::set_n_r,      // SET 4,L
            Registry::set_n_HL,     // SET 4,(HL)
            Registry::set_n_r,      // SET 4,A
            Registry::set_n_r,      // SET 5,B
            Registry::set_n_r,      // SET 5,C
            Registry::set_n_r,      // SET 5,D
            Registry::set_n_r,      // SET 5,E
            Registry::set_n_r,      // SET 5,H
            Registry::set_n_r,      // SET 5,L
            Registry::set_n_HL,     // SET 5,(HL)
            Registry::set_n_r,      // SET 5,A
            Registry::set_n_r,      // SET 6,B
            Registry::set_n_r,      // SET 6,C
            Registry::set_n_r,      // SET 6,D
            Registry::set_n_r,      // SET 6,E
            Registry::set_n_r,      // SET 6,H
            Registry::set_n_r,      // SET 6,L
            Registry::set_n_HL,     // SET 6,(HL)
            Registry::set_n_r,      // SET 6,A
            Registry::set_n_r,      // SET 7,B
            Registry::set_n_r,      // SET 7,C
            Registry::set_n_r,      // SET 7,D
            Registry::set_n_r,      // SET 7,E
            Registry::set_n_r,      // SET 7,H
            Registry::set_n_r,      // SET 7,L
            Registry::set_n_HL,     // SET 7,(HL)
            Registry::set_n_r,      // SET 7,A
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
