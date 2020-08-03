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

#include "mbc_tests.h"

#include <memory/mbc1.h>

// Unit tests in this file are based on ROM/RAM banking examples of the following document:
// https://gekkio.fi/files/gb-docs/gbctr.pdf

#define FB_REG_TEST_MBC1_RAMG 0x0000
#define FB_REG_TEST_MBC1_BANK1 0x2000
#define FB_REG_TEST_MBC1_BANK2 0x4000
#define FB_REG_TEST_MBC1_MODE 0x6000

#define FB_TEST_MBC1_ROM_BANK_SIZE (16 * 1024)

#define FB_TEST_CARTRIDGE_RAM_OFFSET 0xA000

#define assertPointerAddrEquals(expectedPtr, expectedOffset, actualPtr) \
assertEquals(reinterpret_cast<unsigned long>(expectedPtr) + expectedOffset, reinterpret_cast<unsigned long>(actualPtr))

TEST(mbc1RomBankingExample1Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.bank);
    assertEquals(1, mbc1.getROMBank());

    assertEquals(false, mbc1.ramEnabled);
    assertEquals(true, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_BANK1, 0x12);
    mbc1.interceptWrite(FB_REG_TEST_MBC1_BANK2, 0b01);

    assertEquals(0x32, mbc1.bank);
    assertEquals(0x32, mbc1.getROMBank());

    FunkyBoy::u8 dummyRom;
    FunkyBoy::u8 *addr = mbc1.getROMMemoryAddress(0x5000, &dummyRom);

    assertPointerAddrEquals(&dummyRom, (0x5000 - 0x4000) + (0x32 * FB_TEST_MBC1_ROM_BANK_SIZE), addr);

    addr = mbc1.getROMMemoryAddress(0x1000, &dummyRom);
    assertPointerAddrEquals(&dummyRom, 0x1000, addr);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_RAMG, 0b1010);
    assertEquals(true, mbc1.ramEnabled);
    assertEquals(true, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_MODE, 0b1);
    assertEquals(false, mbc1.romBankingMode);

    addr = mbc1.getROMMemoryAddress(0x1000, &dummyRom);
    assertPointerAddrEquals(&dummyRom, 0x1000 + (32 * FB_TEST_MBC1_ROM_BANK_SIZE), addr);
}

TEST(mbc1RomBankingExample2Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.bank);
    assertEquals(1, mbc1.getROMBank());

    assertEquals(false, mbc1.ramEnabled);
    assertEquals(true, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_BANK1, 0b00100);
    mbc1.interceptWrite(FB_REG_TEST_MBC1_BANK2, 0b10);

    assertEquals(0x44, mbc1.bank);
    assertEquals(0x44, mbc1.getROMBank());

    auto dummyRomPtr = reinterpret_cast<FunkyBoy::u8*>(0);
    FunkyBoy::u8 *addr = mbc1.getROMMemoryAddress(0x72A7, dummyRomPtr);

    assertPointerAddrEquals(dummyRomPtr, 0x1132A7, addr);
}

TEST(mbc1RamBankingExample1Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.bank);
    assertEquals(0, mbc1.getRAMBank());

    assertEquals(false, mbc1.ramEnabled);
    assertEquals(true, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_RAMG, 0b1010);
    assertEquals(true, mbc1.ramEnabled);
    assertEquals(true, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_MODE, 0b1);
    assertEquals(false, mbc1.romBankingMode);

    mbc1.interceptWrite(FB_REG_TEST_MBC1_BANK2, 0b10);

    assertEquals(0b1000001, mbc1.bank);
    assertEquals(0b10, mbc1.getRAMBank());

    auto *dummyRamPtr = reinterpret_cast<FunkyBoy::u8*>(0);
    FunkyBoy::u8 *addr = mbc1.getRAMMemoryAddress(0xB123, dummyRamPtr) - FB_TEST_CARTRIDGE_RAM_OFFSET;

    assertPointerAddrEquals(dummyRamPtr, 0x5123, addr);
}

acacia::Report __fbTests_runMbcTests() {
    return runAcaciaFileTests();
}