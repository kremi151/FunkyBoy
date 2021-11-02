/**
 * Copyright 2021 Michel Kremer (kremi151)
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

#include <acacia.h>

#include <emulator/emulator.h>
#include <memory/memory.h>
#include <util/ramsizes.h>
#include <cstring>
#include "../controllers/serial_test.h"
#include "../util/rom_commons.h"
#include <util/membuf.h>

TEST_SUITE(saveStates) {

    template<class T>
    void assertArrayEquals(T expected[], T actual[], size_t size) {
        for (size_t i = 0 ; i < size ; i++) {
            assertEquals(expected[i], actual[i]);
        }
    }

    TEST(testBasicSaveState) {
        auto serial = std::make_shared<FunkyBoy::Controller::SerialControllerTest>();
        FunkyBoy::Emulator emulator1(TEST_GB_TYPE);
        emulator1.setControllers(FunkyBoy::Controller::Controllers().withSerial(serial));
        FunkyBoy::fs::path romPath =
                FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "01-special.gb";
        auto status = emulator1.loadGame(romPath);
        if (status != FunkyBoy::CartridgeStatus::Loaded) {
            testFailure("Loading ROM failed");
        }
        unsigned int i = 0;
        for (; i < 1110000 ; i++) {
            if (!emulator1.doTick()) {
                testFailure("Emulation tick failed");
            }
            if (std::strcmp("Passed", serial->lastWord) == 0) {
                testFailure("ROM test passed too early");
            } else if (std::strcmp("Failed", serial->lastWord) == 0) {
                testFailure("ROM test failed too early");
            }
        }

        FunkyBoy::u8 saveState[FB_SAVE_STATE_MAX_BUFFER_SIZE]{};
        FunkyBoy::Util::membuf outBuf(reinterpret_cast<char *>(saveState), sizeof(saveState), false);
        std::ostream outStream(&outBuf);
        emulator1.saveState(outStream);

        // Reset emulator
        FunkyBoy::Util::membuf inBuf(reinterpret_cast<char *>(saveState), sizeof(saveState), true);
        std::istream inStream(&inBuf);
        FunkyBoy::Emulator emulator2(TEST_GB_TYPE);
        emulator2.setControllers(FunkyBoy::Controller::Controllers().withSerial(serial));
        status = emulator2.loadGame(romPath);
        if (status != FunkyBoy::CartridgeStatus::Loaded) {
            testFailure("Loading ROM in second emulator failed");
        }
        emulator2.loadState(inStream);

        assertEquals(emulator1.memory.ramSizeInBytes, emulator2.memory.ramSizeInBytes);
        assertArrayEquals(emulator1.memory.cram, emulator2.memory.cram, emulator1.memory.ramSizeInBytes);
        assertEquals(emulator1.cpu.instrContext.instr, emulator2.cpu.instrContext.instr);
        assertEquals(emulator1.cpu.instrContext.cbInstr, emulator2.cpu.instrContext.cbInstr);
        assertEquals(emulator1.cpu.instrContext.haltBugRequested, emulator2.cpu.instrContext.haltBugRequested);
        assertEquals(emulator1.cpu.instrContext.interruptMasterEnable, emulator2.cpu.instrContext.interruptMasterEnable);
        assertEquals(emulator1.cpu.instrContext.cpuState, emulator2.cpu.instrContext.cpuState);
        assertEquals(emulator1.cpu.instrContext.lsb, emulator2.cpu.instrContext.lsb);
        assertEquals(emulator1.cpu.instrContext.msb, emulator2.cpu.instrContext.msb);
        assertEquals(emulator1.cpu.instrContext.stackPointer, emulator2.cpu.instrContext.stackPointer);
        assertEquals(emulator1.cpu.instrContext.progCounter, emulator2.cpu.instrContext.progCounter);
        assertEquals(emulator1.cpu.instrContext.signedByte, emulator2.cpu.instrContext.signedByte);
        assertEquals(*emulator1.cpu.instrContext.regA, *emulator2.cpu.instrContext.regA);
        assertEquals(*emulator1.cpu.instrContext.regB, *emulator2.cpu.instrContext.regB);
        assertEquals(*emulator1.cpu.instrContext.regC, *emulator2.cpu.instrContext.regC);
        assertEquals(*emulator1.cpu.instrContext.regD, *emulator2.cpu.instrContext.regD);
        assertEquals(*emulator1.cpu.instrContext.regE, *emulator2.cpu.instrContext.regE);
        assertEquals(*emulator1.cpu.instrContext.regF, *emulator2.cpu.instrContext.regF);

        // Continue executing with second emulator
        for (; i < 2680000 ; i++) {
            if (!emulator2.doTick()) {
                testFailure("Emulation tick failed");
            }
            if (std::strcmp("Passed", serial->lastWord) == 0) {
                std::cout << std::endl;
                break;
            } else if (std::strcmp("Failed", serial->lastWord) == 0) {
                testFailure("Test has failed");
                break;
            }
        }

        // Blargg's test ROMs will print "Passed" if the tests have passed and "Failed" otherwise
        // Mooneye ROMs will output some magic number sequences depending of the success
        assertStandardOutputHasNot("Failed");
        assertStandardOutputHas("Passed");
    }

    void testMBCSaveStateSize(const char *romTitle, FunkyBoy::u8 cartridgeType, FunkyBoy::RAMSize ramSize) {
        FunkyBoy::Emulator emulator(FunkyBoy::GameBoyDMG);

        char rom[0x150]{};
        auto *header = reinterpret_cast<FunkyBoy::ROMHeader *>(rom);

        std::memcpy(header->title, romTitle, std::strlen(romTitle));
        header->cartridgeType = cartridgeType;
        header->ramSize = ramSize;

        FunkyBoy::Util::membuf inBuf(reinterpret_cast<char *>(rom), sizeof(rom), true);
        std::istream inStream(&inBuf);

        emulator.loadGame(inStream);
        assertEquals(emulator.getCartridgeStatus(), FunkyBoy::CartridgeStatus::Loaded);

        emulator.cpu.instrContext.instr = 0x00;

        assertEquals(0x100, emulator.cpu.instrContext.progCounter);

        char saveState[FB_SAVE_STATE_MAX_BUFFER_SIZE];

        // Save the state
        FunkyBoy::Util::membuf outBuf(reinterpret_cast<char *>(saveState), sizeof(saveState), false);
        std::ostream outStream(&outBuf);
        emulator.saveState(outStream);

        // Now load it back to check whether the buffer is large enough
        FunkyBoy::Util::membuf inBuf1(reinterpret_cast<char *>(saveState), sizeof(saveState), true);
        std::istream inStream1(&inBuf1);
        emulator.loadState(inStream1);
    }

    // TODO: https://github.com/kremi151/FunkyBoy/issues/63
    /*TEST(testNoMBCMaxStateSize) {
        testMBCSaveStateSize("NO MBC TEST", 0x09, FunkyBoy::RAMSize::RAM_SIZE_128KB);
    }*/

    TEST(testMBC1MaxStateSize) {
        testMBCSaveStateSize("MBC1 TEST", 0x03, FunkyBoy::RAMSize::RAM_SIZE_32KB);
    }

    TEST(testMBC2MaxStateSize) {
        testMBCSaveStateSize("MBC2 TEST", 0x06, FunkyBoy::RAMSize::RAM_SIZE_128KB);
    }

    TEST(testMBC3MaxStateSize) {
        testMBCSaveStateSize("MBC3 TEST", 0x10, FunkyBoy::RAMSize::RAM_SIZE_64KB);
    }

    TEST(testMBC5MaxStateSize) {
        testMBCSaveStateSize("MBC5 TEST", 0x1E, FunkyBoy::RAMSize::RAM_SIZE_32KB);
    }

}