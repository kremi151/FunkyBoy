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

#include "loads.h"

#include <util/endianness.h>
#include <util/registers.h>

using namespace FunkyBoy;

void Instructions::loadRS(InstrContext &context) {
    u8 &dst = context.registers[context.instr >> 3u & 7u];
    u8 src = context.registers[context.instr & 7];
    dst = src;
}

void Instructions::load_dd_A(InstrContext &context) {
    context.memory->write8BitsTo(Util::compose16Bits(context.lsb, context.msb), *context.regA);
}

void Instructions::load_A_dd(InstrContext &context) {
    *context.regA = context.memory->read8BitsAt(Util::compose16Bits(context.lsb, context.msb));
}

void Instructions::load_A_d(InstrContext &context) {
    *context.regA = context.lsb;
}

void Instructions::load_dd_nn(InstrContext &context) {
    Util::write16BitRegister(context.registers, context.instr >> 4u & 3u, Util::compose16Bits(context.lsb, context.msb));
}

void Instructions::load_SP_nn(InstrContext &context) {
    context.stackPointer = Util::compose16Bits(context.lsb, context.msb);
}

void Instructions::load_nn_SP(InstrContext &context) {
    context.memory->write16BitsTo(Util::compose16Bits(context.lsb, context.msb), context.stackPointer);
}

void Instructions::load_r_n(InstrContext &context) {
    context.registers[context.instr >> 3 & 0b111] = context.lsb;
}

void Instructions::load_HL_n(InstrContext &context) {
    context.memory->write8BitsTo(Util::composeHL(*context.regH, *context.regL), context.lsb);
}