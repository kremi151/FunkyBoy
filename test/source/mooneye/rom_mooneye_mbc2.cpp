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

#include "rom_mooneye_mbc2.h"

#include "commons.h"
#include <util/fs.h>

TEST(testMooneyeMBC2BitsRamg) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "bits_ramg.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2BitsRomb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "bits_romb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2BitsUnused) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "bits_unused.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2Ram) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "ram.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2Rom512Kb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "rom_512kb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2Rom1Mb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "rom_1Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC2Rom2Mb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc2" / "rom_2Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

acacia::Report __fbTests_runMooneyeMBC2RomTests() {
    return runAcaciaFileTests();
}