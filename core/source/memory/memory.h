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

#ifndef FUNKYBOY_CORE_MEMORY_H
#define FUNKYBOY_CORE_MEMORY_H

#include <util/typedefs.h>
#include <cartridge/status.h>
#include <controllers/controllers.h>
#include <emulator/io_registers.h>
#include <memory/ppu_memory.h>
#include <cartridge/mbc.h>

#include <iostream>
#include <cartridge/header.h>

namespace FunkyBoy {

    class Memory {
    private:
        Controller::ControllersPtr controllers;
        io_registers ioRegisters;
        PPUMemory ppuMemory;

        u8 *internalRam;
        u8 *hram;
        u8 interruptEnableRegister;

        u8 dmaMsb{}, dmaLsb{};
        bool dmaStarted;

        u8 *cram;
        size_t ramSizeInBytes;
        size_t romSize;
        CartridgeStatus status;

        std::unique_ptr<MBC> mbc;

        // Do not free these pointers, they are proxies to the ones above:
        u8 *dynamicRamBank;

    test_public:
        u8 *rom;

    public:
        Memory(Controller::ControllersPtr controllers, const io_registers& ioRegisters, const PPUMemory &ppuMemory);
        ~Memory();

        Memory(const Memory &other) = delete;
        Memory &operator= (const Memory &other) = delete;

        void loadROM(std::istream &stream);
        void loadROM(std::istream &stream, bool strictSizeCheck);

        void loadRam(std::istream &stream);
        void writeRam(std::ostream &stream);

        inline size_t getCartridgeRamSize() {
            return ramSizeInBytes;
        }

        const ROMHeader *getROMHeader();
        CartridgeStatus getCartridgeStatus();

        u8 read8BitsAt(memory_address offset);
        i8 readSigned8BitsAt(memory_address offset);
        void write8BitsTo(memory_address offset, u8 val);

        void doDMA();

#ifdef FB_TESTING
        io_registers &getIoRegisters();
#endif
    };

}

#endif //FUNKYBOY_CORE_MEMORY_H
