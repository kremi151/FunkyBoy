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

#ifndef CORE_EMULATOR_H
#define CORE_EMULATOR_H

#include <util/fs.h>
#include <util/return_codes.h>
#include <memory>
#include <emulator/cpu.h>
#include <emulator/ppu.h>
#include <emulator/io_registers.h>
#include <util/typedefs.h>
#include <util/debug.h>
#include <controllers/controllers.h>
#include <memory/memory.h>
#include <memory/ppu_memory.h>
#include <memory>
#include <iostream>

namespace FunkyBoy {

    class Emulator {
    test_public:
        Controller::ControllersPtr controllers;

        io_registers ioRegisters;
        PPUMemory ppuMemory;
        Memory memory;
        CPUPtr cpu;
        PPU ppu;
    public:
        explicit Emulator(GameBoyType gbType);
        Emulator(GameBoyType gbType, const Controller::ControllersPtr &controllers);

        CartridgeStatus loadGame(const fs::path &romPath);
        CartridgeStatus loadGame(std::istream &stream);

        void loadCartridgeRam(std::istream &stream);
        void writeCartridgeRam(std::ostream &stream);

        inline CartridgeStatus getCartridgeStatus() {
            return memory.getCartridgeStatus();
        }

        inline const ROMHeader *getROMHeader() {
            return memory.getROMHeader();
        }

        ret_code doTick();
    };

}

#endif //CORE_EMULATOR_H
