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
#include <util/typedefs.h>
#include <cartridge/rtc.h>
#include <util/ramsizes.h>
#include <cartridge/mbc3.h>
#include "../util/mock_time.h"
#include "../util/membuf.h"

TEST_SUITE(RTCUnitTests) {

    TEST(testRTCLatch) {
        FunkyBoy::RTC rtc;
        rtc.setHours(12);
        rtc.setMinutes(38);
        rtc.setSeconds(42);
        rtc.setDL(69);
        rtc.setDH(1);

        // Everything is 0 as the RTC has not been halted
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setHours(12);
        rtc.setMinutes(38);
        rtc.setSeconds(42);
        rtc.setDL(69);

        assertEquals(12, rtc.getHours() & 0xffffu);
        assertEquals(38, rtc.getMinutes() & 0xffffu);
        assertEquals(42, rtc.getSeconds() & 0xffffu);
        assertEquals(325, rtc.getDays() & 0xffffu);
        assertEquals(69, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);
    }

    TEST(testRTCHaltedSaveLoad) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(42);

        FunkyBoy::RTC rtc;
        rtc.setDH(0b01000000u | 1u); // Halt the RTC so that we can write to the clock counter registers
        rtc.setHours(12);
        rtc.setMinutes(38);
        rtc.setSeconds(42);
        rtc.setDL(69);

        assertTrue(rtc.isHalted());
        assertEquals(12, rtc.getHours() & 0xffffu);
        assertEquals(38, rtc.getMinutes() & 0xffffu);
        assertEquals(42, rtc.getSeconds() & 0xffffu);
        assertEquals(325, rtc.getDays() & 0xffffu);
        assertEquals(69, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);

        FunkyBoy::u8 saveFile[48]{};
        membuf outBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), false);
        std::ostream outStream(&outBuf);
        rtc.write(outStream);

        // Real-life time passes on
        FunkyBoy::Testing::setMockSeconds(45667);

        membuf inBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), true);
        std::istream inStream(&inBuf);

        FunkyBoy::RTC rtc2;
        rtc2.load(inStream);

        assertTrue(rtc2.isHalted());
        assertEquals(12, rtc2.getHours() & 0xffffu);
        assertEquals(38, rtc2.getMinutes() & 0xffffu);
        assertEquals(42, rtc2.getSeconds() & 0xffffu);
        assertEquals(325, rtc2.getDays() & 0xffffu);
        assertEquals(69, rtc2.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc2.getDH() & 0xffffu);

        // Un-halt RTC
        rtc2.setDH(1u);
        assertFalse(rtc2.isHalted());
        assertEquals(12, rtc2.getHours() & 0xffffu);
        assertEquals(38, rtc2.getMinutes() & 0xffffu);
        assertEquals(42, rtc2.getSeconds() & 0xffffu);
        assertEquals(325, rtc2.getDays() & 0xffffu);
        assertEquals(69, rtc2.getDL() & 0xffffu);
        assertEquals(1, rtc2.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(45669);
        assertEquals(12, rtc2.getHours() & 0xffffu);
        assertEquals(38, rtc2.getMinutes() & 0xffffu);
        assertEquals(44, rtc2.getSeconds() & 0xffffu);
        assertEquals(325, rtc2.getDays() & 0xffffu);
        assertEquals(69, rtc2.getDL() & 0xffffu);
        assertEquals(1, rtc2.getDH() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCNonHaltedSaveLoad) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(42);

        FunkyBoy::RTC rtc;

        FunkyBoy::Testing::setMockSeconds(5236865);

        assertFalse(rtc.isHalted());
        assertEquals(14, rtc.getHours() & 0xffffu);
        assertEquals(40, rtc.getMinutes() & 0xffffu);
        assertEquals(23, rtc.getSeconds() & 0xffffu);
        assertEquals(60, rtc.getDays() & 0xffffu);
        assertEquals(60, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::u8 saveFile[48]{};
        membuf outBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), false);
        std::ostream outStream(&outBuf);
        rtc.write(outStream);

        // Real-life time passes on
        FunkyBoy::Testing::setMockSeconds(6964865);

        membuf inBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), true);
        std::istream inStream(&inBuf);

        FunkyBoy::RTC rtc2;
        rtc2.load(inStream);

        assertFalse(rtc2.isHalted());
        assertEquals(14, rtc2.getHours() & 0xffffu);
        assertEquals(40, rtc2.getMinutes() & 0xffffu);
        assertEquals(23, rtc2.getSeconds() & 0xffffu);
        assertEquals(80, rtc2.getDays() & 0xffffu);
        assertEquals(80, rtc2.getDL() & 0xffffu);
        assertEquals(0, rtc2.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(6964866);

        assertFalse(rtc2.isHalted());
        assertEquals(14, rtc2.getHours() & 0xffffu);
        assertEquals(40, rtc2.getMinutes() & 0xffffu);
        assertEquals(24, rtc2.getSeconds() & 0xffffu);
        assertEquals(80, rtc2.getDays() & 0xffffu);
        assertEquals(80, rtc2.getDL() & 0xffffu);
        assertEquals(0, rtc2.getDH() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCTime) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(34);

        FunkyBoy::RTC rtc;

        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(35);

        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(95);

        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(3695);

        assertEquals(1, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(90095);

        assertEquals(1, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(1, rtc.getDays() & 0xffffu);
        assertEquals(1, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(154895);

        assertEquals(19, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(1, rtc.getDays() & 0xffffu);
        assertEquals(1, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        //FunkyBoy::Testing::setMockSeconds(44305295);
        FunkyBoy::Testing::setMockSeconds(44391695);

        assertEquals(19, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);
        assertEquals(1, rtc.getDays() & 0xffffu);
        assertEquals(1, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCLatchTimeContinuity) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(69);

        FunkyBoy::RTC rtc;

        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(82);

        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(13, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        // Halt the RTC
        rtc.setDH(0b01000000u);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0b01000000u, rtc.getDH() & 0xffffu);

        // Time goes on...
        FunkyBoy::Testing::setMockSeconds(186);

        // ...but actually not
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(13, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u, rtc.getDH() & 0xffffu);

        // Un-halt the RTC
        rtc.setDH(0b00000000u);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        // Time goes on again...
        FunkyBoy::Testing::setMockSeconds(242);

        // ...and so does the RTC
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(1, rtc.getMinutes() & 0xffffu);
        assertEquals(9, rtc.getSeconds() & 0xffffu);
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    void testMBC3RTCRegisterSelect(FunkyBoy::RAMSize ramSize) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(69);

        FunkyBoy::MBC3 mbc(FunkyBoy::ROMSize::ROM_SIZE_32K, ramSize, true, true, false);
        auto &rtc = mbc.rtc;

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setHours(12);
        rtc.setMinutes(38);
        rtc.setSeconds(42);
        rtc.setDL(69);

        assertEquals(12, rtc.getHours() & 0xffffu);
        assertEquals(38, rtc.getMinutes() & 0xffffu);
        assertEquals(42, rtc.getSeconds() & 0xffffu);
        assertEquals(325, rtc.getDays() & 0xffffu);
        assertEquals(69, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);

        // Enable RAM & RTC
        mbc.interceptROMWrite(0x0000, 0x0A);

        // Switch to RTC S register
        mbc.interceptROMWrite(0x4000, 0x8);
        assertEquals(42, mbc.readFromRAMAt(0x0000, nullptr) & 0xffffu);

        // Switch to RTC M register
        mbc.interceptROMWrite(0x4000, 0x9);
        assertEquals(38, mbc.readFromRAMAt(0x0000, nullptr) & 0xffffu);

        // Switch to RTC H register
        mbc.interceptROMWrite(0x4000, 0xA);
        assertEquals(12, mbc.readFromRAMAt(0x0000, nullptr) & 0xffffu);

        // Switch to RTC DL register
        mbc.interceptROMWrite(0x4000, 0xB);
        assertEquals(69, mbc.readFromRAMAt(0x0000, nullptr) & 0xffffu);

        // Switch to RTC DL register
        mbc.interceptROMWrite(0x4000, 0xC);
        assertEquals(0b01000000u | 1u, mbc.readFromRAMAt(0x0000, nullptr) & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testMBC3RTCRegisterSelectNoRAM) {
        testMBC3RTCRegisterSelect(FunkyBoy::RAMSize::RAM_SIZE_None);
    }

    TEST(testMBC3RTCRegisterSelect2KBRAM) {
        testMBC3RTCRegisterSelect(FunkyBoy::RAMSize::RAM_SIZE_2KB);
    }

    TEST(testMBC3RTCRegisterSelect8KBRAM) {
        testMBC3RTCRegisterSelect(FunkyBoy::RAMSize::RAM_SIZE_8KB);
    }

    TEST(testMBC3RTCRegisterSelect32KBRAM) {
        testMBC3RTCRegisterSelect(FunkyBoy::RAMSize::RAM_SIZE_32KB);
    }

    TEST(testMBC3RTCRegisterSelect64KBRAM) {
        testMBC3RTCRegisterSelect(FunkyBoy::RAMSize::RAM_SIZE_64KB);
    }

    TEST(testRTCOverflow) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(138);

        FunkyBoy::RTC rtc;

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setDL(255);
        rtc.setHours(23);
        rtc.setMinutes(59);
        rtc.setSeconds(59);

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        rtc.setDH(1); // Un-halt RTC

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(1, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(139); // Let the RTC overflow

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(140);

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(86540);

        assertEquals(1, rtc.getDays() & 0xffffu);
        assertEquals(1, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(44236938);

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u | 1u, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(44236939); // Let the RTC overflow again

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCOverflowWithHalt) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(138);

        FunkyBoy::RTC rtc;

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setDL(255);
        rtc.setHours(23);
        rtc.setMinutes(59);
        rtc.setSeconds(59);

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        rtc.setDH(1); // Un-halt RTC

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(1, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(139); // Let the RTC overflow

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        rtc.setDH(0b11000000u); // Halt the RTC again

        FunkyBoy::Testing::setMockSeconds(140);

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b11000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        rtc.setDH(0b10000000u); // Un-halt RTC

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(141);

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCOverflowWithSaveAndLoad) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(138);

        FunkyBoy::RTC rtc;

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setDL(255);
        rtc.setHours(23);
        rtc.setMinutes(59);
        rtc.setSeconds(59);

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        rtc.setDH(1); // Un-halt RTC

        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(1, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(139); // Let the RTC overflow

        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        FunkyBoy::u8 saveFile[48]{};
        membuf outBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), false);
        std::ostream outStream(&outBuf);
        rtc.write(outStream);

        // Real-life time passes on
        FunkyBoy::Testing::setMockSeconds(420);

        membuf inBuf(reinterpret_cast<char *>(saveFile), sizeof(saveFile), true);
        std::istream inStream(&inBuf);

        FunkyBoy::RTC rtc2;
        rtc2.load(inStream);

        assertEquals(0, rtc2.getDays() & 0xffffu);
        assertEquals(0, rtc2.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc2.getDH() & 0xffffu);
        assertEquals(0, rtc2.getHours() & 0xffffu);
        assertEquals(4, rtc2.getMinutes() & 0xffffu);
        assertEquals(41, rtc2.getSeconds() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

    TEST(testRTCOverflowResetFlag) {
        FunkyBoy::Testing::useMockTime(true);
        FunkyBoy::Testing::setMockSeconds(138);

        FunkyBoy::RTC rtc;
        assertFalse(rtc.isHalted());

        // Now halt the RTC
        rtc.setDH(0b01000000u | 1u);
        rtc.setDL(255);
        rtc.setHours(23);
        rtc.setMinutes(59);
        rtc.setSeconds(59);

        assertTrue(rtc.isHalted());
        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u | 1u, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        rtc.setDH(1); // Un-halt RTC

        assertFalse(rtc.isHalted());
        assertEquals(511, rtc.getDays() & 0xffffu);
        assertEquals(255, rtc.getDL() & 0xffffu);
        assertEquals(1, rtc.getDH() & 0xffffu);
        assertEquals(23, rtc.getHours() & 0xffffu);
        assertEquals(59, rtc.getMinutes() & 0xffffu);
        assertEquals(59, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::setMockSeconds(139); // Let the RTC overflow

        assertFalse(rtc.isHalted());
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b10000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        rtc.setDH(0b11000000u); // Halt the RTC again

        assertTrue(rtc.isHalted());
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b11000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        rtc.setDH(0b01000000u); // Reset the overflow bit

        assertTrue(rtc.isHalted());
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0b01000000u, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(0, rtc.getSeconds() & 0xffffu);

        rtc.setDH(0b00000000); // Un-halt RTC
        assertFalse(rtc.isHalted());

        FunkyBoy::Testing::setMockSeconds(140);

        assertFalse(rtc.isHalted());
        assertEquals(0, rtc.getDays() & 0xffffu);
        assertEquals(0, rtc.getDL() & 0xffffu);
        assertEquals(0, rtc.getDH() & 0xffffu);
        assertEquals(0, rtc.getHours() & 0xffffu);
        assertEquals(0, rtc.getMinutes() & 0xffffu);
        assertEquals(1, rtc.getSeconds() & 0xffffu);

        FunkyBoy::Testing::useMockTime(false);
    }

}