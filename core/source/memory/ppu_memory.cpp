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

#include <exception/read_exception.h>

using namespace FunkyBoy;

#define FB_VRAM_BYTES 8192
#define FB_OAM_BYTES 160

PPUMemory::PPUMemory()
    : vram(new u8[FB_VRAM_BYTES]{})
    , oam(new u8[FB_OAM_BYTES]{})
    , vramAccessible(new bool(true))
    , oamAccessible(new bool(true))
    , ptrCounter(new u16(1))
{
}

PPUMemory::PPUMemory(const PPUMemory &other)
    : vram(other.vram)
    , oam(other.oam)
    , vramAccessible(other.vramAccessible)
    , oamAccessible(other.oamAccessible)
    , ptrCounter(other.ptrCounter)
{
    (*ptrCounter)++;
}

PPUMemory::~PPUMemory() {
    if (--(*ptrCounter) < 1) {
        delete[] vram;
        delete[] oam;
        delete vramAccessible;
        delete oamAccessible;
        delete ptrCounter;
    }
}

void PPUMemory::setAccessibilityFromMMU(bool accessVram, bool accessOam) {
    *vramAccessible = accessVram;
    *oamAccessible = accessOam;
}

void PPUMemory::serialize(std::ostream &ostream) const {
    ostream.put(*vramAccessible);
    ostream.put(*oamAccessible);
    ostream.write(reinterpret_cast<const char*>(vram), FB_VRAM_BYTES);
    ostream.write(reinterpret_cast<const char*>(oam), FB_OAM_BYTES);
}

void PPUMemory::deserialize(std::istream &istream) {
    char buffer[2];
    istream.read(buffer, sizeof(buffer));
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }

    *vramAccessible = buffer[0] != 0;
    *oamAccessible = buffer[1] != 0;

    istream.read(reinterpret_cast<char*>(vram), FB_VRAM_BYTES);
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }
    istream.read(reinterpret_cast<char*>(oam), FB_OAM_BYTES);
    if (!istream) {
        throw Exception::ReadException("Stream is too short");
    }
}