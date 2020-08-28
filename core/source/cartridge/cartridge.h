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
#include <util/testing.h>
#include <cartridge/header.h>
#include <memory/mbc.h>

#include <iostream>
#include <memory>

namespace FunkyBoy {

    class Memory;

    enum CartridgeStatus {
        NoROMLoaded,
        ROMFileNotReadable,
        ROMParseError,
        ROMTooBig,
        ROMSizeMismatch,
        ROMUnsupportedMBC,
        RAMSizeUnsupported,
        Loaded
    };

    enum CartridgeRAMSize {
        CRAM_None = 0x0,
        CRAM_2KB = 0x1,
        CRAM_8KB = 0x2,
        CRAM_32KB = 0x3,
        CRAM_128KB = 0x4,
        CRAM_64KB = 0x5
    };

    class Cartridge {
    private:
        u8 *ram;
        size_t romSize;
        CartridgeStatus status;

        std::unique_ptr<MBC> mbc;

        friend class Memory;

    test_public:
        u8 *rom;

    public:
        Cartridge();
        ~Cartridge();

        void loadROM(std::istream &stream);
        void loadROM(std::istream &stream, bool strictSizeCheck);

        void loadRam(std::istream &stream);
        void writeRam(std::ostream &stream);

        size_t getRamSize();

        const ROMHeader *getHeader();
        CartridgeStatus getStatus();
    };

    typedef std::shared_ptr<Cartridge> CartridgePtr;

}

#endif //FUNKYBOY_CORE_CARTRIDGE_H
