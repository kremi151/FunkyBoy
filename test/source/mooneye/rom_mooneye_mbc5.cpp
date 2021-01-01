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

#include "rom_mooneye_mbc1.h"

#include "commons.h"
#include <util/fs.h>

TEST(testMooneyeMBC5Rom512Kb) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_512kb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom1MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_1Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom2MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_2Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom4MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_4Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom8MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_8Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom16MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_16Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom32MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_32Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

TEST(testMooneyeMBC5Rom64MB) {
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "emulator-only" / "mbc5" / "rom_64Mb.gb";
    testUsingMooneyeROM(romPath, 5120000);
}

acacia::Report __fbTests_runMooneyeMBC5RomTests() {
    return runAcaciaFileTests();
}