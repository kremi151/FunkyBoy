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

#include "rom_mooneye_mbc1.h"

#include "util/mooneye_common.h"
#include <util/fs.h>

TEST(testMooneyeMBC1Rom512Kb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_512kb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Rom1MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_1Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Rom2MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_2Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Rom4MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_4Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Rom8MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_8Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Rom16MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "rom_16Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Ram64Kb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "ram_64Kb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1Ram256Kb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "ram_256Kb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1BitsBank1) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "bits_bank1.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1BitsBank2) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "bits_bank2.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC1BitsRAMG) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "bits_ramg.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

// TODO: Fix test for bits_mode.gb
/*TEST(testMooneyeMBC1BitsMode) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc1" / "bits_mode.gb";
    testUsingMooneyeROM(romPath, 5120000);
}*/

// TODO: Add test for multicart_rom_8Mb.gb

acacia::Report __fbTests_runMooneyeMBC1RomTests() {
    return runAcaciaFileTests();
}