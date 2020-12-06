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
