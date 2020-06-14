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
#include <memory>

#define FB_MEMORY_ADDR_INTERRUPT_ENABLE_REGISTER 0xFFFF

namespace FunkyBoy {

    class Memory {
    private:
        std::shared_ptr<Cartridge> cartridge;
        u8 *vram; // Character RAM
        u8 *bgMapData1;
        u8 *bgMapData2;
        u8 *internalRam;
        u8 *oam;
        u8 *hwIO;
        u8 *hram;
        u8 interruptEnableFlag;

        // Do not free these pointers, they are proxies to the ones above:
        u8 *dynamicRamBank;

        u8 *getMemoryAddress(memory_address offset);

        bool interceptWrite(memory_address offset, u8 val);
    public:
        explicit Memory(std::shared_ptr<Cartridge> cartridge);
        ~Memory();

        u8 read8BitsAt(memory_address offset);
        i8 readSigned8BitsAt(memory_address offset);
        void write8BitsTo(memory_address offset, u8 val);
        void incrementAt(memory_address offset);
        void decrementAt(memory_address offset);

        u16 read16BitsAt(memory_address offset);

        [[deprecated]]
        void write16BitsTo(memory_address offset, u16 val);
        void write16BitsTo(memory_address offset, u8 msb, u8 lsb);
    };

}

#endif //FUNKYBOY_CORE_MEMORY_H
