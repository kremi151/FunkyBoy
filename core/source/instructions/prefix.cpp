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

#include <instructions/reads.h>
#include <instructions/writes.h>

using namespace FunkyBoy;

bool __prefix__rlc_r(InstrContext &context) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg << 1) | ((*reg >> 7) & 0b1);
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
    *reg = newVal;
    return true;
}

bool __prefix__rlc_lsb(InstrContext &context) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal << 1) | ((oldVal >> 7) & 0b1);
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix__rrc_r(InstrContext &context) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg >> 1) | ((*reg & 0b1) << 7);
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b1) > 0);
    *reg = newVal;
    return true;
}

bool __prefix__rrc_lsb(InstrContext &context) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal >> 1) | ((oldVal & 0b1) << 7);
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b1) > 0);
    context.lsb = newVal;
    return true;
}

bool __prefix_rl_r(InstrContext &context) {
    u8 *reg = context.registers + (context.instr & 0b111);
    u8 newVal = (*reg << 1);
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b1;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
    *reg = newVal;
    return true;
}

bool __prefix_rl_HL(InstrContext &context) {
    u8 oldVal = context.lsb;
    u8 newVal = (oldVal << 1);
    if (Flags::isCarry(context.regF)) {
        newVal |= 0b1;
    }
    Flags::setFlags(context.regF, newVal == 0, false, false, (oldVal & 0b10000000) > 0);
    context.lsb = newVal;
    return true;
}

bool Instructions::decodePrefix(InstrContext &context) {
    context.instr = context.memory->read8BitsAt(context.progCounter++);
    switch (context.instr) {
        // rlc reg
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
            debug_print_4("rlc r\n");
            context.operands[1] = __prefix__rlc_r;
            context.operands[2] = nullptr;
            return true;
        }
        // rlc (HL)
        case 0x06: {
            debug_print_4("rlc (HL)\n");
            context.operands[1] = Instructions::readHLMem;
            context.operands[2] = __prefix__rlc_lsb;
            context.operands[3] = Instructions::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
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
            context.operands[1] = Instructions::readHLMem;
            context.operands[2] = __prefix__rrc_lsb;
            context.operands[3] = Instructions::writeLSBIntoHLMem;
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
            context.operands[1] = Instructions::readHLMem;
            context.operands[2] = __prefix_rl_HL;
            context.operands[3] = Instructions::writeLSBIntoHLMem;
            context.operands[4] = nullptr;
            return true;
        }
    }
    return true;
}
