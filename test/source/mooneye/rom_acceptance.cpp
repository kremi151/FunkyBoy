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

#include "rom_acceptance.h"

#include "commons.h"

TEST(testMooneyeAcceptanceBitsMemOam) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "acceptance" / "bits" / "mem_oam.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeAcceptanceBitsRegF) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "acceptance" / "bits" / "reg_f.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

// TODO: Fix test
/*TEST(testMooneyeAcceptanceBitsUnusedHwIoGS) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "acceptance" / "bits" / "unused_hwio-GS.gb";
    testUsingMooneyeROM(romPath, 5120000);
}*/

TEST(testMooneyeAcceptanceInstrDaa) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "acceptance" / "instr" / "daa.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

acacia::Report __fbTests_runMooneyeAcceptanceRomTests() {
    return runAcaciaFileTests();
}
