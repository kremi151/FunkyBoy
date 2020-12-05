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

#include "context.h"

using namespace FunkyBoy::Instructions;

context::context(GameBoyType gbType)
    : gbType(gbType)
{
}

void context::write16BitRegister(FunkyBoy::u8_fast position, FunkyBoy::u16_fast val) {
    u8_fast *reg = registers + (position * 2);
    *reg = (val >> 8u) & 0xffu;
    *(reg + 1) = val & 0xffu;
}

FunkyBoy::u16_fast context::read16BitRegister(FunkyBoy::u8_fast position) {
    u8_fast *reg = registers + (position * 2);
    return (*reg << 8u) | (*(reg + 1u) & 0xffu);
}

void context::writeHL(FunkyBoy::u16_fast val) {
    *regL = val & 0xffu;
    *regH = (val >> 8u) & 0xffu;
}

void context::push16Bits(FunkyBoy::Memory &memory, u16_fast val) {
    stackPointer -= 2;
    memory.write8BitsTo(stackPointer, val & 0xffu);
    memory.write8BitsTo(stackPointer + 1, (val >> 8u) & 0xffu);
}

FunkyBoy::u16_fast context::pop16Bits(FunkyBoy::Memory &memory) {
    u16_fast val = memory.read8BitsAt(stackPointer) | (memory.read8BitsAt(stackPointer + 1) << 8u);
    stackPointer += 2;
    return val;
}