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
#include <exception/read_exception.h>

// TODO: For debugging, remove it afterwards:
#include <sstream>

using namespace FunkyBoy;

Emulator::Emulator(GameBoyType gbType, const Controller::ControllersPtr& controllers)
    : controllers(controllers)
    , ioRegisters(controllers)
    , ppuMemory()
    , memory(controllers, ioRegisters, ppuMemory)
    , cpu(std::make_shared<CPU>(gbType, ioRegisters))
    , ppu(cpu, controllers, ioRegisters, ppuMemory)
{
    // Initialize registers
    cpu->powerUpInit(memory);
}

Emulator::Emulator(FunkyBoy::GameBoyType gbType): Emulator(
        gbType,
        std::make_shared<Controller::Controllers>()
) {}

CartridgeStatus Emulator::loadGame(const fs::path &romPath) {
    std::ifstream romFile(romPath.c_str(), std::ios::binary);
    return loadGame(romFile);
}

CartridgeStatus Emulator::loadGame(std::istream &stream) {
    memory.loadROM(stream);

    if (memory.getCartridgeStatus() != CartridgeStatus::Loaded) {
        std::cerr << "ROM could not be loaded, status " << memory.getCartridgeStatus() << std::endl;
        return memory.getCartridgeStatus();
    }

    auto header = memory.getROMHeader();

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

    return memory.getCartridgeStatus();
}

void Emulator::loadCartridgeRam(std::istream &stream) {
    memory.loadRam(stream);
}

void Emulator::writeCartridgeRam(std::ostream &stream) {
    memory.writeRam(stream);
}

#define FB_SAVE_STATE_VERSION 1

void Emulator::saveState(std::ostream &ostream) {
    ostream.put(FB_SAVE_STATE_VERSION);
    // TODO: GB Type
    cpu->serialize(ostream);

    // TODO: ioRegisters
    // TODO: ppuMemory
    // TODO: memory
    // TODO: MBC state
}

void Emulator::loadState(std::istream &istream) {
    int version = istream.get();
    if (version != FB_SAVE_STATE_VERSION) {
        throw Exception::ReadException("Save state version mismatch");
    }
    cpu->deserialize(istream);
}

ret_code Emulator::doTick() {
    auto result = cpu->doMachineCycle(memory);
    if (!result) {
        return 0;
    }
    result |= ppu.doClocks(4);
    return result;
}
