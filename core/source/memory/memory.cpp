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

#include "memory.h"

using namespace FunkyBoy;

#define FB_INTERNAL_RAM_BANK_SIZE (4 * 1024)

Memory::Memory(std::shared_ptr<Cartridge> cartridge): cartridge(std::move(cartridge)), interruptEnableFlag(0) {
    restartInterruptVectorTable = new u8[255]{};
    vram = new u8[6144]{};
    bgMapData1 = new u8[1024]{};
    bgMapData2 = new u8[1024]{};
    internalRam = new u8[8 * FB_INTERNAL_RAM_BANK_SIZE]{};
    oam = new u8[160]{};
    hwIO = new u8[128]{};
    hram = new u8[127]{};
}

Memory::~Memory() {
    delete[] restartInterruptVectorTable;
    delete[] vram;
    delete[] bgMapData1;
    delete[] bgMapData2;
    delete[] internalRam;
    delete[] oam;
    delete[] hwIO;
    delete[] hram;
}