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

#include <filesystem>
#include <memory>
#include <emulator/cpu.h>
#include <emulator/io_registers.h>
#include <util/typedefs.h>
#include <util/debug.h>
#include <cartridge/cartridge.h>
#include <controllers/serial.h>
#include <memory/memory.h>
#include <memory>

namespace FunkyBoy {

    class Emulator {
    test_public:
        CartridgePtr cartridge;
        Controller::SerialControllerPtr serialController;

        io_registers_ptr ioRegisters;
        MemoryPtr memory;
        CPU cpu;
    public:
        explicit Emulator(GameBoyType gbType);
        Emulator(GameBoyType gbType, const Controller::SerialControllerPtr &serialController);

        CartridgeStatus loadGame(const std::filesystem::path &romPath);
        Cartridge &getCartridge();

        bool doTick();
    };

}

#endif //CORE_EMULATOR_H
