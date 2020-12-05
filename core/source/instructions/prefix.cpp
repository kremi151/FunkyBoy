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
        u8_fast *reg = context.registers + (opcode & 0b111);
        u8_fast newVal = ((*reg << 1) | ((*reg >> 7) & 0b1)) & 0xffu;
        Flags::setFlagsFast(*context.regF, newVal == 0, false, false, (*reg & 0b10000000) > 0);
        *reg = newVal;
    }

}

int Prefix::execute(opcode_t opcode, context &context, Memory &memory) {
    switch (opcode) {
        /* rlc reg */ case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
            debug_print_4("rlc r\n");
            Prefix::rlc_r(opcode, context, memory);
            return 8;
        }
        default: {
            fprintf(stderr, "Encountered not yet implemented prefix 0x%02X at 0x%04X\n", opcode, context.progCounter - 1);
            return 0;
        }
    }
}