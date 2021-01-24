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
    private:
#ifdef FB_USE_AUTOSAVE
        int cramLastWritten;

        void doAutosave();
#endif
    test_public:
        Controller::ControllersPtr controllers;

        io_registers ioRegisters;
        PPUMemory ppuMemory;
        Memory memory;
        CPUPtr cpu;
        PPU ppu;
    public:
#ifdef FB_USE_AUTOSAVE
        fs::path savePath;
#endif

        explicit Emulator(GameBoyType gbType);
        Emulator(GameBoyType gbType, const Controller::ControllersPtr &controllers);

        CartridgeStatus loadGame(const fs::path &romPath);
        CartridgeStatus loadGame(std::istream &stream);

        void loadCartridgeRam(std::istream &stream);
        void writeCartridgeRam(std::ostream &stream);

        void loadState(std::istream &istream);
        void saveState(std::ostream &ostream);

        inline void setInputState(Controller::JoypadKey key, bool pressed) {
            ioRegisters.setInputState(key, pressed);
        }

        inline CartridgeStatus getCartridgeStatus() {
            return memory.getCartridgeStatus();
        }

        inline const ROMHeader *getROMHeader() {
            return memory.getROMHeader();
        }

        inline u8 *releaseROM(size_t *size) {
            return memory.releaseROM(size);
        }

        inline size_t getCartridgeRamSize() {
            return memory.getCartridgeRamSize();
        }

        inline bool supportsSaving() {
            return memory.getCartridgeRamSize() > 0;
        }

        ret_code doTick();
    };

}

#endif //CORE_EMULATOR_H
