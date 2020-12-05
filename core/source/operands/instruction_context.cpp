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

#include "instruction_context.h"

using namespace FunkyBoy;

InstrContext::InstrContext(FunkyBoy::GameBoyType gbType): gbType(gbType) {
}

void InstrContext::push16Bits(Memory &memory, u16 val) {
    stackPointer -= 2;
    memory.write8BitsTo(stackPointer, (u8) (val));
    memory.write8BitsTo(stackPointer + 1, (u8) (val >> 8u));
}

void InstrContext::push16Bits(Memory &memory, u8 inMsb, u8 inLsb) {
    stackPointer -= 2;
    memory.write8BitsTo(stackPointer, inLsb);
    memory.write8BitsTo(stackPointer + 1, inMsb);
}

u16 InstrContext::pop16Bits(Memory &memory) {
    u16 val = memory.read8BitsAt(stackPointer) | (memory.read8BitsAt(stackPointer + 1) << 8u);
    stackPointer += 2;
    return val;
}

void InstrContext::writeHL(u16 val) {
    *regL = val & 0xffu;
    *regH = (val >> 8u) & 0xffu;
}

void InstrContext::write16BitRegister(u8 position, u16 val) {
    u8 *reg = registers + (position * 2);
    *reg = (val >> 8u) & 0xffu;
    *(reg + 1) = val & 0xffu;
}

u16 InstrContext::read16BitRegister(u8 position) {
    u8 *reg = registers + (position * 2);
    return (*reg << 8u) | (*(reg + 1u) & 0xffu);
}