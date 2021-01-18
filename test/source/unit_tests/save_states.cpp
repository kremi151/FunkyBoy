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
#include <cstring>
#include "../controllers/serial_test.h"
#include "../util/rom_commons.h"
#include "../util/membuf.h"

// Buffer size needs to be large enough, exact size doesn't matter
#define SAVE_STATE_BUFFER_SIZE (64 * 1024)

TEST_SUITE(saveStates) {

    template<class T>
    void assertArrayEquals(T expected[], T actual[], size_t size) {
        for (size_t i = 0 ; i < size ; i++) {
            assertEquals(expected[i], actual[i]);
        }
    }

    TEST(testBasicSaveState) {
        auto controllers = std::make_shared<FunkyBoy::Controller::Controllers>();
        auto serial = std::make_shared<FunkyBoy::Controller::SerialControllerTest>();
        controllers->setSerial(serial);
        FunkyBoy::Emulator emulator1(
                TEST_GB_TYPE,
                controllers
        );
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

        FunkyBoy::u8 saveState[SAVE_STATE_BUFFER_SIZE]{};
        membuf outBuf(reinterpret_cast<char *>(saveState), sizeof(saveState), false);
        std::ostream outStream(&outBuf);
        emulator1.saveState(outStream);

        // Reset emulator
        membuf inBuf(reinterpret_cast<char *>(saveState), sizeof(saveState), true);
        std::istream inStream(&inBuf);
        FunkyBoy::Emulator emulator2(
                TEST_GB_TYPE,
                controllers
        );
        emulator2.loadState(inStream);

        assertEquals(emulator1.memory.ramSizeInBytes, emulator2.memory.ramSizeInBytes);
        assertArrayEquals(emulator1.memory.cram, emulator2.memory.cram, emulator1.memory.ramSizeInBytes);
        assertEquals(emulator1.cpu->instrContext.instr, emulator2.cpu->instrContext.instr);
        assertEquals(emulator1.cpu->instrContext.cbInstr, emulator2.cpu->instrContext.cbInstr);
        assertEquals(emulator1.cpu->instrContext.haltBugRequested, emulator2.cpu->instrContext.haltBugRequested);
        assertEquals(emulator1.cpu->instrContext.interruptMasterEnable, emulator2.cpu->instrContext.interruptMasterEnable);
        assertEquals(emulator1.cpu->instrContext.cpuState, emulator2.cpu->instrContext.cpuState);
        assertEquals(emulator1.cpu->instrContext.lsb, emulator2.cpu->instrContext.lsb);
        assertEquals(emulator1.cpu->instrContext.msb, emulator2.cpu->instrContext.msb);
        assertEquals(emulator1.cpu->instrContext.stackPointer, emulator2.cpu->instrContext.stackPointer);
        assertEquals(emulator1.cpu->instrContext.progCounter, emulator2.cpu->instrContext.progCounter);
        assertEquals(emulator1.cpu->instrContext.signedByte, emulator2.cpu->instrContext.signedByte);
        assertEquals(*emulator1.cpu->instrContext.regA, *emulator2.cpu->instrContext.regA);
        assertEquals(*emulator1.cpu->instrContext.regB, *emulator2.cpu->instrContext.regB);
        assertEquals(*emulator1.cpu->instrContext.regC, *emulator2.cpu->instrContext.regC);
        assertEquals(*emulator1.cpu->instrContext.regD, *emulator2.cpu->instrContext.regD);
        assertEquals(*emulator1.cpu->instrContext.regE, *emulator2.cpu->instrContext.regE);
        assertEquals(*emulator1.cpu->instrContext.regF, *emulator2.cpu->instrContext.regF);

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

}