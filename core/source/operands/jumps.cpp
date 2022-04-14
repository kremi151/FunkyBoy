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

#ifndef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include "jumps.h"

#include <util/endianness.h>
#include <util/debug.h>

using namespace FunkyBoy;

bool Operands::jp(InstrContext &context, Memory &memory) {
    debug_print_4("JP from 0x%04X", context.progCounter);
    context.progCounter = Util::compose16Bits(context.lsb, context.msb);
    debug_print_4(" to 0x%04X\n", context.progCounter);
    return true;
}

bool Operands::jp_HL(InstrContext &context, Memory &memory) {
    debug_print_4("JP (HL) from 0x%04X", context.progCounter);
    context.progCounter = context.readHL();
    debug_print_4(" to 0x%04X\n", context.progCounter);
    return true;
}

bool Operands::jr(InstrContext &context, Memory &memory) {
    debug_print_4("JR from 0x%04X + %d", context.progCounter, context.signedByte);
    context.progCounter += context.signedByte;
    debug_print_4(" to 0x%04X\n", context.progCounter);
    return true;
}

bool Operands::call(InstrContext &context, Memory &memory) {
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    debug_print_4("call from 0x%04X\n", context.progCounter);
    context.push16Bits(memory, context.progCounter);
    context.progCounter = address;
    debug_print_4(" to 0x%04X\n", context.progCounter);
    return true;
}

bool Operands::ret(InstrContext &context, Memory &memory) {
    context.progCounter = context.pop16Bits(memory);
    return true;
}

bool Operands::rst(InstrContext &context, Memory &memory) {
    u8 rstAddr = (context.instr >> 3u & 7u) * 8u;
    debug_print_4("rst %02XH\n", rstAddr);
    context.push16Bits(memory, context.progCounter);
    context.progCounter = rstAddr;
    return true;
}

#endif
