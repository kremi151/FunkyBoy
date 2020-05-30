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

#include "emulator.h"

#include <iostream>
#include <fstream>
#include <cartridge/header.h>

// TODO: For debugging, remove it afterwards:
#include <sstream>

using namespace FunkyBoy;

Emulator::Emulator(): cartridge(new Cartridge), cpu(cartridge) {
    vram = new u8[16384];
}

Emulator::~Emulator() {
    delete[] vram;
}

void Emulator::loadGame(const std::filesystem::path &romPath) {
    std::ifstream romFile(romPath.c_str(), std::ios::binary);

    cartridge->loadROM(romFile);

    if (cartridge->getStatus() != CartridgeStatus::Loaded) {
        std::cerr << "ROM could not be loaded" << std::endl;
        return; // TODO: Give more descriptive errors
    }

    auto header = cartridge->getHeader();

    cpu.setProgramCounter(FB_ROM_HEADER_ENTRY_POINT);

    std::cout << "Nintendo logo:" << std::endl;
    std::stringstream ss;
    for (u8 i = 0 ; i < 48 ; i++) {
        if (i != 0 && i % 16 == 0) {
            ss << std::endl;
        }
        ss << std::hex << static_cast<unsigned int>(header->nintendoLogo[i] & 0xff);
        ss << " ";
    }
    std::cout << ss.str() << std::endl;
}

bool Emulator::doTick() {
    return cpu.doTick();
}