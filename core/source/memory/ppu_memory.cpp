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

#include "ppu_memory.h"

#include <util/endianness.h>

using namespace FunkyBoy;

PPUMemory::PPUMemory()
    : vram(new u8[8192]{})
    , oam(new u8[160]{})
    , ptrCounter(new u16(1))
{
}

PPUMemory::PPUMemory(const PPUMemory &other)
    : vram(other.vram)
    , oam(other.oam)
    , ptrCounter(other.ptrCounter)
{
    (*ptrCounter)++;
}

PPUMemory::~PPUMemory() {
    if (--(*ptrCounter) < 1) {
        delete[] vram;
        delete[] oam;
        delete ptrCounter;
    }
}

u8 &PPUMemory::getVRAMByte(memory_address vramOffset) {
    return *(vram + vramOffset);
}

u16 PPUMemory::readVRAM16Bits(memory_address vramOffset) {
    return Util::compose16Bits(*(vram + vramOffset), *(vram + vramOffset + 1));
}

u8 &PPUMemory::getOAMByte(memory_address oamOffset) {
    return *(oam + oamOffset);
}
