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

#include "alu.h"

#include <util/typedefs.h>
#include <util/flags.h>

using namespace FunkyBoy;

inline void __alu_and(u8 *flags, u8 *regA, u8 val) {
    *regA &= val;
    //TODO: To be verified:
    Flags::setFlags(flags, *regA == 0, false, true, false);
}

inline void __alu_xor(u8 *flags, u8 *regA, u8 val) {
    *regA ^= val;
    Flags::setFlags(flags, *regA == 0, false, false, false);
}

bool Operands::and_r(InstrContext &context, Memory &memory) {
    // 0xA0 -> 10100 000 -> B
    // 0xA1 -> 10100 001 -> C
    // 0xA2 -> 10100 010 -> D
    // 0xA3 -> 10100 011 -> E
    // 0xA4 -> 10100 100 -> H
    // 0xA5 -> 10100 101 -> L
    // -- F is skipped --
    // 0xA7 -> 10100 111 -> A
    __alu_and(context.regF, context.regA, context.registers[context.instr & 0b111u]);
    return true;
}

bool Operands::and_d(InstrContext &context, Memory &memory) {
    __alu_and(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::and_HL(InstrContext &context, Memory &memory) {
    __alu_and(context.regF, context.regA, memory.read8BitsAt(context.readHL()));
    return true;
}

bool Operands::xor_r(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, context.registers[context.instr & 0b111u]);
    return true;
}

bool Operands::xor_d(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, context.lsb);
    return true;
}

bool Operands::xor_HL(InstrContext &context, Memory &memory) {
    __alu_xor(context.regF, context.regA, memory.read8BitsAt(context.readHL()));
    return true;
}