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

#include "rom_cpu_instrs_tests.h"

#include "commons.h"
#include <util/fs.h>

TEST(testCPUInstructionsSpecial) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "01-special.gb";
    testUsingBlarggROM(romPath, 2680000);
}

TEST(testCPUInstructionsInterrupts) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "02-interrupts.gb";
    testUsingBlarggROM(romPath, 1000000);
}

TEST(testCPUInstructionsOpSPHL) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "03-op sp,hl.gb";
    testUsingBlarggROM(romPath, 5120000);
}

TEST(testCPUInstructionsOpRImm) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "04-op r,imm.gb";
    testUsingBlarggROM(romPath, 8120000);
}

TEST(testCPUInstructionsOpRP) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "05-op rp.gb";
    testUsingBlarggROM(romPath, 8810000);
}

TEST(testCPUInstructionsLoads) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "06-ld r,r.gb";
    testUsingBlarggROM(romPath, 2004455);
}

TEST(testCPUInstructionsJrJpCallRetRst) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "07-jr,jp,call,ret,rst.gb";
    testUsingBlarggROM(romPath, 2000000);
}

TEST(testCPUInstructionsMisc) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "08-misc instrs.gb";
    testUsingBlarggROM(romPath, 1500000);
}

TEST(testCPUInstructionsOpRR) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "09-op r,r.gb";
    testUsingBlarggROM(romPath, 32000000);
}

TEST(testCPUInstructionsBitOps) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "10-bit ops.gb";
    testUsingBlarggROM(romPath, 32000000);
}

TEST(testCPUInstructionsOpAHL) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "11-op a,(hl).gb";
    testUsingBlarggROM(romPath, 40000000);
}

// TODO: Fix and re-enable (issue #7)
/*TEST(testROMHaltBug) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "halt_bug.gb";
    testUsingBlarggROM(romPath, 40000000);
}*/

acacia::Report __fbTests_runBlarggCpuInstrsRomTests() {
    return runAcaciaFileTests();
}