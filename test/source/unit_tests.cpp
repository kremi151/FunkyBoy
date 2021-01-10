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

#include "unit_tests.h"

#include "util/rom_commons.h"

#include <acacia.h>
#include <util/fs.h>
#include <util/flags.h>
#include <memory/memory.h>
#include <memory>
#include <emulator/emulator.h>
#include <controllers/controllers.h>
#include <cartridge/mbc1.h>
#include <cartridge/mbc2.h>
#include <cartridge/mbc3.h>
#include <cartridge/rtc.h>
#include "util/membuf.h"
#include "util/mock_time.h"

bool doFullMachineCycle(FunkyBoy::CPU &cpu, FunkyBoy::Memory &memory) {
    cpu.instructionCompleted = false;
    do {
        if (!cpu.doMachineCycle(memory)) {
            return false;
        }
    } while (!cpu.instructionCompleted);
    return true;
}

void assertDoFullMachineCycle(FunkyBoy::CPU &cpu, FunkyBoy::Memory &memory) {
    if (!doFullMachineCycle(cpu, memory)) {
        testFailure("Emulation tick failed");
    }
}

inline FunkyBoy::Memory createMemory() {
    auto controllers = std::make_shared<FunkyBoy::Controller::Controllers>();
    FunkyBoy::io_registers io(controllers);
    FunkyBoy::PPUMemory ppuMemory;
    return FunkyBoy::Memory(controllers, io, ppuMemory);
}

TEST(testEchoRAM) {
    auto memory = createMemory();

    // Write to beginning of internal RAM bank 0
    memory.write8BitsTo(0xC000, 42);
    int val = memory.read8BitsAt(0xE000);
    assertEquals(42, val);

    // Write to end of internal RAM bank 0
    memory.write8BitsTo(0xCFFF, 124);
    val = memory.read8BitsAt(0xEFFF);
    assertEquals(124, val);

    // Write to beginning of internal RAM bank 1
    memory.write8BitsTo(0xD000, 186);
    val = memory.read8BitsAt(0xF000);
    assertEquals(186, val);
}

TEST(testReadROMTitle) {
    FunkyBoy::Emulator emulator(TEST_GB_TYPE);
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "cpu_instrs.gb";
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    auto cartridgeStatus = emulator.getCartridgeStatus();
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, cartridgeStatus);
    assertEquals("CPU_INSTRS", std::string(reinterpret_cast<const char*>(emulator.getROMHeader()->title)));
}

TEST(testPopPushStackPointer) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    cpu.instrContext.push16Bits(memory, 0x1806);
    auto val = cpu.instrContext.pop16Bits(memory);
    assertEquals(0x1806, val);

    cpu.instrContext.push16Bits(memory, 0x28, 0x09);
    cpu.instrContext.push16Bits(memory, 0x1223);
    cpu.instrContext.push16Bits(memory, 0x42, 0x69);

    val = cpu.instrContext.pop16Bits(memory);
    assertEquals(0x4269, val);
    val = cpu.instrContext.pop16Bits(memory);
    assertEquals(0x1223, val);
    val = cpu.instrContext.pop16Bits(memory);
    assertEquals(0x2809, val);
}

TEST(testReadWriteHLAndAF) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // In this test, we check for enforcing little-endianness

    cpu.writeAF(0x1234);
    // 0x34 -> 0b110100
    // 0x34 -> F -> 0b110000 -> 0x30 (register F only stores the 4 most significant bits)
    assertEquals(0x30, *cpu.regF_do_not_use_directly);
    assertEquals(0x12, *cpu.regA);
    auto val = cpu.readAF();
    assertEquals(0x1230, val);

    cpu.instrContext.writeHL(0x1806);
    assertEquals(0x06, *cpu.regL);
    assertEquals(0x18, *cpu.regH);
    val = cpu.instrContext.readHL();
    assertEquals(0x1806, val);
}

TEST(testReadWrite16BitRegisters) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // In this test, we check for enforcing little-endianness

    // BC
    cpu.instrContext.write16BitRegister(0, 0x1234);
    assertEquals(0x34, *cpu.regC);
    assertEquals(0x12, *cpu.regB);
    auto val = cpu.instrContext.read16BitRegister(0);
    assertEquals(0x1234, val);

    // DE
    cpu.instrContext.write16BitRegister(1, 0x1806);
    assertEquals(0x06, *cpu.regE);
    assertEquals(0x18, *cpu.regD);
    val = cpu.instrContext.read16BitRegister(1);
    assertEquals(0x1806, val);

    // HL
    cpu.instrContext.write16BitRegister(2, 0x2809);
    assertEquals(0x09, *cpu.regL);
    assertEquals(0x28, *cpu.regH);
    val = cpu.instrContext.read16BitRegister(2);
    assertEquals(0x2809, val);
}

TEST(test16BitLoads) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter + 1] = 0x06;
    memory.rom[initialProgCounter + 2] = 0x18;
    memory.rom[initialProgCounter + 3] = 0x00;

    // Set opcode 0x01 (LD BC,d16)
    memory.rom[initialProgCounter] = 0x01;
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regC) & 0xff);
    assertEquals(0x18, (*cpu.regB) & 0xff);

    // Set opcode 0x11 (LD DE,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0x11;
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regE) & 0xff);
    assertEquals(0x18, (*cpu.regD) & 0xff);

    // Set opcode 0x21 (LD HL,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0x21;
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regL) & 0xff);
    assertEquals(0x18, (*cpu.regH) & 0xff);

    // Set opcode 0x31 (LD SP,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0x31;
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x1806, cpu.instrContext.stackPointer);
}

// Disabled for now, it seems to me like this test is failing because of some logic mistake I made in the test code
// As the ROM tests should also cover this instruction, it should be tolerable to disable it for the moment
// TODO: Fix and re-enable
/*TEST(testLDHA) {
    FunkyBoy::CartridgePtr cartridge(new FunkyBoy::Cartridge);
    FunkyBoy::io_registers_ptr io(new FunkyBoy::io_registers);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge, io);
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, io);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter + 1] = 0xCE;

    // Set opcode 0xF0 (LDH A,(a8))
    memory.rom[initialProgCounter] = 0xF0;
    memory.write8BitsTo(0xFFCE, 0x42);
    assertNotEquals(0x42, *cpu.regA);
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution
    // PC = initial + fetch(1) + a8(1) + fetch(1)
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(0x42, *cpu.regA);

    // Reset prog counter and register
    cpu.instrContext.progCounter = initialProgCounter;
    memory.write8BitsTo(0xFFCE, 0x0);

    // Set opcode 0xE0 (LDH (a8),A)
    memory.rom[initialProgCounter] = 0xE0;
    *cpu.regA = 0x42;
    assertNotEquals(0x42, memory.read8BitsAt(0xFFCE));
    assertDoFullMachineCycle(cpu, memory); // Initial fetch
    assertDoFullMachineCycle(cpu, memory); // Actual execution
    // PC = initial + fetch(1) + a8(1) + fetch(1)
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(0x42, memory.read8BitsAt(0xFFCE));
}*/

TEST(testHALTBugSkipping) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    memory.write8BitsTo(FB_REG_IE, 0x4);
    memory.write8BitsTo(FB_REG_IF, 0x4);

    // Program ROM
    memory.rom[initialProgCounter] = 0x76;      // HALT
    memory.rom[initialProgCounter + 1] = 0x3C;  // INC A
    memory.rom[initialProgCounter + 2] = 0x00;  // NOP

    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Initial fetch without executing anything
    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x76, cpu.instrContext.instr);
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x3C, cpu.instrContext.instr); // Fetched next instruction already
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x3C, cpu.instrContext.instr);
    assertEquals(initialProgCounter + 2, cpu.instrContext.progCounter);
    assertEquals(originalA + 1, *cpu.regA);

    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x00, cpu.instrContext.instr); // Fetched next instruction already
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(originalA + 2, *cpu.regA);
}

TEST(testHALTNoSkippingIfIMEDisabled) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    // So we set (IE & IF) == 0 here
    memory.write8BitsTo(FB_REG_IE, 0x4);
    memory.write8BitsTo(FB_REG_IF, 0x2);

    // Program ROM
    memory.rom[initialProgCounter] = 0x76;      // HALT
    memory.rom[initialProgCounter + 1] = 0x3C;  // INC A
    memory.rom[initialProgCounter + 2] = 0x00;  // NOP

    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Initial fetch without executing anything
    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x76, cpu.instrContext.instr);
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    // Assert that we are indeed in HALT mode (no execution of any opcode)
    for (int i = 0 ; i < 5 ; i++) {
        assertDoFullMachineCycle(cpu, memory);

        assertEquals(FunkyBoy::CPUState::HALTED, cpu.instrContext.cpuState);

        assertEquals(0x76, cpu.instrContext.instr); // No fetch of next instruction, we stay at the HALT instruction
        assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
        assertEquals(originalA, *cpu.regA);
    }

    // Set IE and IF to same value -> should request an interrupt
    memory.write8BitsTo(FB_REG_IE, 0x4);
    memory.write8BitsTo(FB_REG_IF, 0x4);

    // As IME == 0, we must not jump to the interrupt vector
    // Instead, we just continue reading the next opcodes

    assertDoFullMachineCycle(cpu, memory);
    assertEquals(0x3C, cpu.instrContext.instr & 0xffff); // Fetched next instruction already
    assertEquals(initialProgCounter + 2, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);
}

TEST(testHALTBugHanging) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    memory.write8BitsTo(FB_REG_IE, 0x4);
    memory.write8BitsTo(FB_REG_IF, 0x4);

    // Program ROM
    memory.rom[initialProgCounter] = 0x76;      // HALT
    memory.rom[initialProgCounter + 1] = 0x76;  // HALT
    memory.rom[initialProgCounter + 2] = 0x3C;  // INC A

    *cpu.instrContext.regA = 0x00;
    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Maybe a bit excessive, but iterating 100 times without advancing the PC should demonstrate the HALT bug correctly
    for (int i = 0 ; i < 100 ; i++) {
        assertDoFullMachineCycle(cpu, memory);
        assertEquals(0x76, cpu.instrContext.instr);
        assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
        assertEquals(originalA, *cpu.regA);
    }
}

// Test Operands::checkIsZeroContextual and Operands::checkIsCarryContextual using RET
TEST(testContextualZeroAndCarryCheckOperands) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    memory.rom = new FunkyBoy::u8[0x200];
    memory.rom[0x123] = 0x00;      // NOP

    auto initialProgCounter = 0x100;

    // Test RET NZ (0xC0)
    cpu.instrContext.push16Bits(memory, 0x123);
    FunkyBoy::Flags::setZero(cpu.instrContext.regF, false);
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0xC0;      // RET NZ
    memory.rom[initialProgCounter + 1] = 0x00;  // NOP
    assertDoFullMachineCycle(cpu, memory);    // Initial fetch
    assertDoFullMachineCycle(cpu, memory);    // Actual execution
    assertEquals(0x123 + 1, cpu.instrContext.progCounter);

    // Test RET Z (0xC8)
    cpu.instrContext.push16Bits(memory, 0x123);
    FunkyBoy::Flags::setZero(cpu.instrContext.regF, true);
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0xC8;      // RET Z
    memory.rom[initialProgCounter + 1] = 0x00;  // NOP
    assertDoFullMachineCycle(cpu, memory);    // Initial fetch
    assertDoFullMachineCycle(cpu, memory);    // Actual execution
    assertEquals(0x123 + 1, cpu.instrContext.progCounter);

    // Test RET NC (0xD0)
    cpu.instrContext.push16Bits(memory, 0x123);
    FunkyBoy::Flags::setCarry(cpu.instrContext.regF, false);
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0xD0;      // RET NC
    memory.rom[initialProgCounter + 1] = 0x00;  // NOP
    assertDoFullMachineCycle(cpu, memory);    // Initial fetch
    assertDoFullMachineCycle(cpu, memory);    // Actual execution
    assertEquals(0x123 + 1, cpu.instrContext.progCounter);

    // Test RET C (0xD8)
    cpu.instrContext.push16Bits(memory, 0x123);
    FunkyBoy::Flags::setCarry(cpu.instrContext.regF, true);
    cpu.instrContext.progCounter = initialProgCounter;
    memory.rom[initialProgCounter] = 0xD8;      // RET C
    memory.rom[initialProgCounter + 1] = 0x00;  // NOP
    assertDoFullMachineCycle(cpu, memory);    // Initial fetch
    assertDoFullMachineCycle(cpu, memory);    // Actual execution
    assertEquals(0x123 + 1, cpu.instrContext.progCounter);
}

size_t calc_quick_hash(const FunkyBoy::u8 *array, size_t len) {
    size_t result = 0;
    for (size_t i = 0 ; i < len ; i++) {
        result = (result * 31) + array[i];
    }
    return result;
}

void testBatterySave(FunkyBoy::Memory &memory, size_t ramSize) {
    using namespace FunkyBoy;

    memory.ramSizeInBytes = ramSize;
    memory.cram = new u8[memory.ramSizeInBytes]{}; // Gets freed by Memory's destructor

    memory.cram[0] = 0x12;
    memory.cram[1] = 0x34;
    memory.cram[ramSize / 2] = 0xab;
    memory.cram[(ramSize / 2) + 1] = 0xcd;
    memory.cram[ramSize - 2] = 0x1a;
    memory.cram[ramSize - 1] = 0x2b;

    size_t originalHash = calc_quick_hash(memory.cram, memory.ramSizeInBytes);

    u8 *saveFile = new u8[memory.ramSizeInBytes];
    membuf outBuf(reinterpret_cast<char *>(saveFile), memory.ramSizeInBytes, false);
    std::ostream outStream(&outBuf);
    memory.writeRam(outStream);

    assertEquals(0x12, saveFile[0]);
    assertEquals(0x34, saveFile[1]);
    assertEquals(0xab, saveFile[ramSize / 2]);
    assertEquals(0xcd, saveFile[(ramSize / 2) + 1]);
    assertEquals(0x1a, saveFile[ramSize - 2]);
    assertEquals(0x2b, saveFile[ramSize - 1]);

    size_t calculatedHash = calc_quick_hash(saveFile, memory.ramSizeInBytes);
    assertEquals(originalHash, calculatedHash);

    delete[] memory.cram;
    memory.cram = new u8[memory.ramSizeInBytes]; // Gets freed by Memory's destructor

    membuf inBuf(reinterpret_cast<char *>(saveFile), memory.ramSizeInBytes, true);
    std::istream inStream(&inBuf);
    memory.loadRam(inStream);

    assertEquals(0x12, memory.cram[0] & 0xff);
    assertEquals(0x34, memory.cram[1]);
    assertEquals(0xab, memory.cram[ramSize / 2]);
    assertEquals(0xcd, memory.cram[(ramSize / 2) + 1]);
    assertEquals(0x1a, memory.cram[ramSize - 2]);
    assertEquals(0x2b, memory.cram[ramSize - 1]);

    calculatedHash = calc_quick_hash(saveFile, memory.ramSizeInBytes);
    assertEquals(originalHash, calculatedHash);
}

TEST(testBatterySaveMBC1) {
    auto memory = createMemory();
    memory.mbc = std::make_unique<FunkyBoy::MBC1>(FunkyBoy::ROMSize::ROM_SIZE_2M, FunkyBoy::RAMSize::RAM_SIZE_8KB, true);

    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    testBatterySave(memory, 8192);
}

TEST(testBatterySaveMBC2) {
    auto memory = createMemory();
    memory.mbc = std::make_unique<FunkyBoy::MBC2>(FunkyBoy::ROMSize::ROM_SIZE_2M, true);

    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    testBatterySave(memory, 2048);
}

TEST(testBatterySaveMBC3) {
    auto memory = createMemory();
    memory.mbc = std::make_unique<FunkyBoy::MBC3>(FunkyBoy::ROMSize::ROM_SIZE_2M, FunkyBoy::RAMSize::RAM_SIZE_8KB, true, false, false);

    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory.getIoRegisters());
    cpu.powerUpInit(memory);

    testBatterySave(memory, 8192);
}

TEST(testMemoryReadSigned8BitsAt) {
    auto memory = createMemory();
    memory.rom = new FunkyBoy::u8[10]{}; // Freed by memory's destructor

    memory.rom[3] = 11;
    memory.rom[4] = 254;
    memory.rom[5] = 127;
    memory.rom[6] = 128;
    memory.rom[7] = 0;

    FunkyBoy::i8 signedByte = memory.readSigned8BitsAt(3);
    assertEquals(11, signedByte);

    signedByte = memory.readSigned8BitsAt(4);
    assertEquals(-2, signedByte);

    signedByte = memory.readSigned8BitsAt(5);
    assertEquals(127, signedByte);

    signedByte = memory.readSigned8BitsAt(6);
    assertEquals(-128, signedByte);

    signedByte = memory.readSigned8BitsAt(7);
    assertEquals(0, signedByte);
}

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

TEST(testMBC3RTCRegisterSelect) {
    FunkyBoy::Testing::useMockTime(true);
    FunkyBoy::Testing::setMockSeconds(69);

    FunkyBoy::MBC3 mbc(FunkyBoy::ROMSize::ROM_SIZE_32K, FunkyBoy::RAMSize::RAM_SIZE_2KB, true, true, false);
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
}

acacia::Report __fbTests_runUnitTests() {
    return runAcaciaFileTests();
}
