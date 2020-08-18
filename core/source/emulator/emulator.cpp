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
#include <emulator/gb_type.h>
#include <cartridge/header.h>

// TODO: For debugging, remove it afterwards:
#include <sstream>

using namespace FunkyBoy;

Emulator::Emulator(GameBoyType gbType, const Controller::ControllersPtr& controllers)
    : cartridge(new Cartridge)
    , controllers(controllers)
    , ioRegisters(controllers)
    , ppuMemory()
    , memory(new Memory(cartridge, controllers, ioRegisters, ppuMemory))
    , cpu(std::make_shared<CPU>(gbType, memory, ioRegisters))
    , ppu(cpu, controllers, ioRegisters, ppuMemory)
{
}

Emulator::Emulator(FunkyBoy::GameBoyType gbType): Emulator(
        gbType,
        std::make_shared<Controller::Controllers>()
) {}

CartridgeStatus Emulator::loadGame(const fs::path &romPath) {
    std::ifstream romFile(romPath.c_str(), std::ios::binary);

    cartridge->loadROM(romFile);

    if (cartridge->getStatus() != CartridgeStatus::Loaded) {
        std::cerr << "ROM could not be loaded from " << romPath.string() << std::endl;
        return cartridge->getStatus();
    }

    auto header = cartridge->getHeader();

    cpu->setProgramCounter(FB_ROM_HEADER_ENTRY_POINT);

#ifdef FB_DEBUG
    fprintf(stdout, "Cartridge type: 0x%02X\n", header->cartridgeType);

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
#endif

    savePath = romPath;
    savePath.replace_extension(".sav");

    return cartridge->getStatus();
}

#ifndef __ANDROID__

void Emulator::loadCartridgeRamFromFS() {
    if (fs::exists(savePath)) {
        std::ifstream file(savePath.c_str(), std::ios::binary);
        cartridge->loadRamFromFS(file);
    }
}

void Emulator::writeCartridgeRamToFS() {
    std::ofstream file(savePath.c_str(), std::ios::binary);
    cartridge->writeRamToFS(file);
}

#endif

ret_code Emulator::doTick() {
    auto result = cpu->doMachineCycle();
    if (!result) {
        return 0;
    }
    result |= ppu.doClocks(4);
    return result;
}

Cartridge & Emulator::getCartridge() {
    return *cartridge;
}