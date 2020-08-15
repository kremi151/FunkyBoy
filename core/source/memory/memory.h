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
#include <cartridge/cartridge.h>
#include <controllers/controllers.h>
#include <emulator/io_registers.h>
#include <memory/ppu_memory.h>

namespace FunkyBoy {

    class Memory {
    private:
        CartridgePtr cartridge;
        Controller::ControllersPtr controllers;
        io_registers ioRegisters;
        PPUMemory ppuMemory;

        u8 *internalRam;
        u8 *hram;
        u8 interruptEnableRegister;

        u8 dmaMsb{}, dmaLsb{};
        bool dmaStarted;

        // Do not free these pointers, they are proxies to the ones above:
        u8 *dynamicRamBank;

        u8 *getMemoryAddress(memory_address offset);

        bool interceptWrite(memory_address offset, u8 &val);
        bool interceptReadAt(memory_address offset, u8 *out);
    public:
        Memory(CartridgePtr cartridge, Controller::ControllersPtr controllers, const io_registers& ioRegisters, const PPUMemory &ppuMemory);
        ~Memory();

        Memory(const Memory &other) = delete;
        Memory &operator= (const Memory &other) = delete;

        u8 read8BitsAt(memory_address offset);
        i8 readSigned8BitsAt(memory_address offset);
        void write8BitsTo(memory_address offset, u8 val);

        u16 read16BitsAt(memory_address offset);

        void write16BitsTo(memory_address offset, u16 val);
        void write16BitsTo(memory_address offset, u8 msb, u8 lsb);

        fb_inline bool isDMA();
        void doDMA();

#ifdef FB_TESTING
        io_registers &getIoRegisters();
        CartridgePtr &getCartridge();
#endif
    };

    typedef std::shared_ptr<Memory> MemoryPtr;

}

#endif //FUNKYBOY_CORE_MEMORY_H
