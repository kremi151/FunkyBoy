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

using namespace FunkyBoy;

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

bool Operands::decodePrefix(InstrContext &context, Memory &memory) {
    context.instr = memory.read8BitsAt(context.progCounter++);

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG
    FunkyBoy::Debug::writeExecutionToLog('P', *context.executionLog, context);
#endif

    switch (context.instr) {
        // rrc reg
        case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: {
            debug_print_4("rrc r\n");
            context.operands[1] = __prefix__rrc_r;
            context.operands[2] = nullptr;
            return true;
        }
        // rrc (HL)
        case 0x0E: {
            debug_print_4("rrc (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix__rrc_lsb;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // rl reg
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: {
            debug_print_4("rl r\n");
            context.operands[1] = __prefix_rl_r;
            context.operands[2] = nullptr;
            return true;
        }
        // rl (HL)
        case 0x16: {
            debug_print_4("rl (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_rl_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // rr reg
        case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1F: {
            debug_print_4("rr r\n");
            context.operands[1] = __prefix_rr_r;
            context.operands[2] = nullptr;
            return true;
        }
        // rr (HL)
        case 0x1E: {
            debug_print_4("rr (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_rr_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // sla reg
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27: {
            debug_print_4("sla r\n");
            context.operands[1] = __prefix_sla_r;
            context.operands[2] = nullptr;
            return true;
        }
        // sla (HL)
        case 0x26: {
            debug_print_4("sla (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_sla_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // sra reg
        case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F: {
            debug_print_4("sra r\n");
            context.operands[1] = __prefix_sra_r;
            context.operands[2] = nullptr;
            return true;
        }
        // sra (HL)
        case 0x2E: {
            debug_print_4("sra (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_sra_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // swap reg
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: {
            debug_print_4("swap r\n");
            context.operands[1] = __prefix_swap_r;
            context.operands[2] = nullptr;
            return true;
        }
        // swap (HL)
        case 0x36: {
            debug_print_4("swap (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_swap_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        // srl reg
        case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3F: {
            debug_print_4("srl r\n");
            context.operands[1] = __prefix_srl_r;
            context.operands[2] = nullptr;
            return true;
        }
        // srl (HL)
        case 0x3E: {
            debug_print_4("srl (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_srl_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // bit 0,reg
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F: // bit 1,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // bit 2,reg
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F: // bit 3,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // bit 4,reg
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: // bit 5,reg
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: // bit 6,reg
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: // bit 7,reg
        {
            debug_print_4("bit r\n");
            context.operands[1] = __prefix_bit_r;
            context.operands[2] = nullptr;
            return true;
        }
        // bit n,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x76: case 0x7E: {
            debug_print_4("srl (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_bit_HL;
            context.operands[3] = Operands::_pad_;
            context.operands[4] = nullptr;
            return true;
        }
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // res 0,reg
        case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F: // res 1,reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // res 2,reg
        case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // res 3,reg
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: // res 4,reg
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: // res 5,reg
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: // res 6,reg
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: // res 7,reg
        {
            debug_print_4("res r\n");
            context.operands[1] = __prefix_res_r;
            context.operands[2] = nullptr;
            return true;
        }
        // res n,(HL)
        case 0x86: case 0x8E: case 0x96: case 0x9E: case 0xA6: case 0xAE: case 0xB6: case 0xBE: {
            debug_print_4("res (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_res_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7: // set 0,reg
        case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF: // set 1,reg
        case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7: // set 2,reg
        case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF: // set 3,reg
        case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7: // set 4,reg
        case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF: // set 5,reg
        case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7: // set 6,reg
        case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF: // set 7,reg
        {
            debug_print_4("set r\n");
            context.operands[1] = __prefix_set_r;
            context.operands[2] = nullptr;
            return true;
        }
        // set n,(HL)
        case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE: {
            debug_print_4("set (HL)\n");
            context.operands[1] = Operands::readHLMem;
            context.operands[2] = __prefix_set_HL;
            context.operands[3] = Operands::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", context.instr, context.progCounter - 1);
            return false;
        }
    }
    return true;
}
