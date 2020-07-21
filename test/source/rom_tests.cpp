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

#include "rom_tests.h"

#include "commons.h"

#include <acacia.h>
#include <memory/memory.h>
#include <emulator/emulator.h>

void testUsingROM(const std::filesystem::path &romPath, unsigned int expectedTicks) {
    FunkyBoy::Emulator emulator(TEST_GB_TYPE);
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    for (unsigned int i = 0 ; i < expectedTicks ; i++) {
        if (!emulator.doTick()) {
            testFailure("Emulation tick failed");
        }
    }

    // Blargg's test ROMs will print "Passed" if the tests have passed and "Failed" otherwise
    assertStandardOutputHasNot("Failed");
    assertStandardOutputHas("Passed");
}

TEST(testCPUInstructionsSpecial) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "01-special.gb";
        testUsingROM(romPath, 5120000);
}

// TODO: 02-interrupts.gb

TEST(testCPUInstructionsOpSPHL) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "03-op sp,hl.gb";
        testUsingROM(romPath, 5120000);
}

TEST(testCPUInstructionsOpRImm) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "04-op r,imm.gb";
        testUsingROM(romPath, 8120000);
}

TEST(testCPUInstructionsOpRP) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "05-op rp.gb";
        testUsingROM(romPath, 8810000);
}

TEST(testCPUInstructionsLoads) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "06-ld r,r.gb";
        testUsingROM(romPath, 2004455);
}

TEST(testCPUInstructionsJrJpCallRetRst) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "07-jr,jp,call,ret,rst.gb";
        testUsingROM(romPath, 2000000);
}

TEST(testCPUInstructionsMisc) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "08-misc instrs.gb";
        testUsingROM(romPath, 1500000);
}

TEST(testCPUInstructionsOpRR) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "09-op r,r.gb";
        testUsingROM(romPath, 32000000);
}

TEST(testCPUInstructionsBitOps) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "10-bit ops.gb";
        testUsingROM(romPath, 32000000);
}

TEST(testCPUInstructionsOpAHL) {
        std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "11-op a,(hl).gb";
        testUsingROM(romPath, 40000000);
}

acacia::Report __fbTests_runRomTests() {
    return runAcaciaFileTests();
}