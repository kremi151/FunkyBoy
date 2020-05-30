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

#ifndef FUNKYBOY_CORE_CARTRIDGE_H
#define FUNKYBOY_CORE_CARTRIDGE_H

#include <util/typedefs.h>
#include <cartridge/header.h>

#include <iostream>

namespace FunkyBoy {

    class Memory;

    enum CartridgeStatus {
        NoROMLoaded,
        ROMFileNotReadable,
        ROMParseError,
        ROMTooBig,
        ROMSizeMismatch,
        Loaded
    };

    class Cartridge {
    private:
        u8 *rom;
        u8 *ram;
        size_t romSize;
        CartridgeStatus status;

        friend class Memory;

    public:
        Cartridge();
        ~Cartridge();

        void loadROM(std::ifstream &file);

        const ROMHeader *getHeader();
        CartridgeStatus getStatus();
        u8 instructionAt(u32 offset);
        u16 read16BitsAt(u32 offset);
    };

}

#endif //FUNKYBOY_CORE_CARTRIDGE_H
