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
#include <memory>

// Unit tests in this file are based on ROM/RAM banking examples of the following document:
// https://gekkio.fi/files/gb-docs/gbctr.pdf

#define FB_REG_TEST_MBC1_RAMG 0x0000
#define FB_REG_TEST_MBC1_BANK1 0x2000
#define FB_REG_TEST_MBC1_BANK2 0x4000
#define FB_REG_TEST_MBC1_MODE 0x6000

#define FB_TEST_MBC1_ROM_BANK_SIZE (16 * 1024)

#define FB_TEST_CARTRIDGE_RAM_OFFSET 0xA000

//#define assertPointerAddrEquals(expectedPtr, expectedOffset, actualPtr) \
//assertEquals(reinterpret_cast<unsigned long>(expectedPtr) + expectedOffset, reinterpret_cast<unsigned long>(actualPtr))

TEST(mbc1RomBankingExample1Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::ROMSize::ROM_SIZE_4096K, FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.romBank);

    assertFalse(mbc1.ramEnabled);
    assertFalse(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_BANK1, 0x12);
    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_BANK2, 0b01);

    assertEquals(0x32, mbc1.romBank);

    std::unique_ptr<FunkyBoy::u8[]> rom(new FunkyBoy::u8[(0x32 + 1) * FB_TEST_MBC1_ROM_BANK_SIZE]{});

    // Place some specific magic numbers in the ROM so that we can check if we're at the correct address
    *(rom.get() + (0x5000 - FB_REG_TEST_MBC1_BANK2) + (0x32 * FB_TEST_MBC1_ROM_BANK_SIZE)) = 42;
    *(rom.get() + (0x5000 - FB_REG_TEST_MBC1_BANK2) + (0x20 * FB_TEST_MBC1_ROM_BANK_SIZE)) = 69;
    *(rom.get() + 0x1000) = 18;

    FunkyBoy::u8 val = mbc1.readFromROMAt(0x5000, rom.get());
    assertEquals(42, val);

    val = mbc1.readFromROMAt(0x1000, rom.get());
    assertEquals(18, val);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_RAMG, 0b1010);
    assertTrue(mbc1.ramEnabled);
    assertFalse(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_MODE, 0b1);
    assertTrue(mbc1.ramBankingMode);

    val = mbc1.readFromROMAt(0x1000, rom.get());
    assertEquals(69, val);
}

TEST(mbc1RomBankingExample2Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::ROMSize::ROM_SIZE_4096K, FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.romBank);

    assertFalse(mbc1.ramEnabled);
    assertFalse(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_BANK1, 0b00100);
    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_BANK2, 0b10);

    assertEquals(0x44, mbc1.romBank);

    std::unique_ptr<FunkyBoy::u8[]> rom(new FunkyBoy::u8[0x1132A7]{});

    // Place some specific magic numbers in the ROM so that we can check if we're at the correct address
    *(rom.get() + 0x1132A7) = 11;

    FunkyBoy::u8 val = mbc1.readFromROMAt(0x72A7, rom.get());
    assertEquals(11, val);
}

TEST(mbc1RamBankingExample1Test) {
    FunkyBoy::MBC1 mbc1(FunkyBoy::ROMSize::ROM_SIZE_4096K, FunkyBoy::MBC1RAMSize::MBC1_32KByte);

    assertEquals(1, mbc1.romBank);
    assertEquals(0, mbc1.ramBank);

    assertFalse(mbc1.ramEnabled);
    assertFalse(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_RAMG, 0b1010);
    assertTrue(mbc1.ramEnabled);
    assertFalse(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_MODE, 0b1);
    assertTrue(mbc1.ramBankingMode);

    mbc1.interceptROMWrite(FB_REG_TEST_MBC1_BANK2, 0b10);

    assertEquals(0x41, mbc1.romBank);
    assertEquals(0b10, mbc1.ramBank);

    std::unique_ptr<FunkyBoy::u8[]> ram(new FunkyBoy::u8[0x5123]{});

    // Place some specific magic numbers in the RAM so that we can check if we're at the correct address
    *(ram.get() + 0x5123) = 99;

    FunkyBoy::u8 val = mbc1.readFromRAMAt(0xB123 - FB_TEST_CARTRIDGE_RAM_OFFSET, ram.get());
    assertEquals(99, val);
}

acacia::Report __fbTests_runMbcTests() {
    return runAcaciaFileTests();
}