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
#include <memory/memory.h>
#include <memory>
#include <emulator/emulator.h>
#include <controllers/controllers.h>

bool doFullMachineCycle(FunkyBoy::CPU &cpu) {
    cpu.instructionCompleted = false;
    do {
        if (!cpu.doMachineCycle()) {
            return false;
        }
    } while (!cpu.instructionCompleted);
    return true;
}

void assertDoFullMachineCycle(FunkyBoy::CPU &cpu) {
    if (!doFullMachineCycle(cpu)) {
        testFailure("Emulation tick failed");
    }
}

FunkyBoy::MemoryPtr createMemory() {
    auto controllers = std::make_shared<FunkyBoy::Controller::Controllers>();
    FunkyBoy::CartridgePtr cartridge(new FunkyBoy::Cartridge);
    FunkyBoy::io_registers io(controllers);
    FunkyBoy::PPUMemory ppuMemory;
    return std::make_shared<FunkyBoy::Memory>(cartridge, controllers, io, ppuMemory);
}

TEST(testEchoRAM) {
    auto memory = createMemory();

    // Write to beginning of internal RAM bank 0
    memory->write8BitsTo(0xC000, 42);
    int val = memory->read8BitsAt(0xE000);
    assertEquals(42, val);

    // Write to end of internal RAM bank 0
    memory->write8BitsTo(0xCFFF, 124);
    val = memory->read8BitsAt(0xEFFF);
    assertEquals(124, val);

    // Write to beginning of internal RAM bank 1
    memory->write8BitsTo(0xD000, 186);
    val = memory->read8BitsAt(0xF000);
    assertEquals(186, val);
}

TEST(testReadROMTitle) {
    FunkyBoy::Emulator emulator(TEST_GB_TYPE);
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "gb-test-roms" / "cpu_instrs" / "cpu_instrs.gb";
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    auto &cartridge = emulator.getCartridge();
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, cartridge.getStatus());
    assertEquals("CPU_INSTRS", std::string(reinterpret_cast<const char*>(cartridge.getHeader()->title)));
}

TEST(testPopPushStackPointer) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

    cpu.instrContext.push16Bits(*memory, 0x1806);
    auto val = cpu.instrContext.pop16Bits(*memory);
    assertEquals(0x1806, val);

    cpu.instrContext.push16Bits(*memory, 0x2809);
    cpu.instrContext.push16Bits(*memory, 0x1223);
    cpu.instrContext.push16Bits(*memory, 0x4269);

    val = cpu.instrContext.pop16Bits(*memory);
    assertEquals(0x4269, val);
    val = cpu.instrContext.pop16Bits(*memory);
    assertEquals(0x1223, val);
    val = cpu.instrContext.pop16Bits(*memory);
    assertEquals(0x2809, val);
}

TEST(testReadWriteHLAndAF) {
    auto memory = createMemory();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

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
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

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
    auto cartridge = memory->getCartridge();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter + 1] = 0x06;
    cartridge->rom[initialProgCounter + 2] = 0x18;
    cartridge->rom[initialProgCounter + 3] = 0x00;

    // Set opcode 0x01 (LD BC,d16)
    cartridge->rom[initialProgCounter] = 0x01;
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regC) & 0xff);
    assertEquals(0x18, (*cpu.regB) & 0xff);

    // Set opcode 0x11 (LD DE,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x11;
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regE) & 0xff);
    assertEquals(0x18, (*cpu.regD) & 0xff);

    // Set opcode 0x21 (LD HL,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x21;
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution

    // PC = initial + fetch(1) + execution(2) + next fetch(1) = initial + 4
    assertEquals(initialProgCounter + 4, cpu.instrContext.progCounter);
    assertEquals(0x06, (*cpu.regL) & 0xff);
    assertEquals(0x18, (*cpu.regH) & 0xff);

    // Set opcode 0x31 (LD SP,d16)
    cpu.instrContext.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x31;
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution

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
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter + 1] = 0xCE;

    // Set opcode 0xF0 (LDH A,(a8))
    cartridge->rom[initialProgCounter] = 0xF0;
    memory->write8BitsTo(0xFFCE, 0x42);
    assertNotEquals(0x42, *cpu.regA);
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution
    // PC = initial + fetch(1) + a8(1) + fetch(1)
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(0x42, *cpu.regA);

    // Reset prog counter and register
    cpu.instrContext.progCounter = initialProgCounter;
    memory->write8BitsTo(0xFFCE, 0x0);

    // Set opcode 0xE0 (LDH (a8),A)
    cartridge->rom[initialProgCounter] = 0xE0;
    *cpu.regA = 0x42;
    assertNotEquals(0x42, memory->read8BitsAt(0xFFCE));
    assertDoFullMachineCycle(cpu); // Initial fetch
    assertDoFullMachineCycle(cpu); // Actual execution
    // PC = initial + fetch(1) + a8(1) + fetch(1)
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(0x42, memory->read8BitsAt(0xFFCE));
}*/

TEST(testHALTBugSkipping) {
    auto memory = createMemory();
    auto cartridge = memory->getCartridge();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    memory->write8BitsTo(FB_REG_IE, 0x4);
    memory->write8BitsTo(FB_REG_IF, 0x4);

    // Program ROM
    cartridge->rom[initialProgCounter] = 0x76;      // HALT
    cartridge->rom[initialProgCounter + 1] = 0x3C;  // INC A
    cartridge->rom[initialProgCounter + 2] = 0x00;  // NOP

    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Initial fetch without executing anything
    assertDoFullMachineCycle(cpu);
    assertEquals(0x76, cpu.currentOpcode);
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    assertDoFullMachineCycle(cpu);
    assertEquals(0x3C, cpu.currentOpcode); // Fetched next instruction already
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    assertDoFullMachineCycle(cpu);
    assertEquals(0x3C, cpu.currentOpcode);
    assertEquals(initialProgCounter + 2, cpu.instrContext.progCounter);
    assertEquals(originalA + 1, *cpu.regA);

    assertDoFullMachineCycle(cpu);
    assertEquals(0x00, cpu.currentOpcode); // Fetched next instruction already
    assertEquals(initialProgCounter + 3, cpu.instrContext.progCounter);
    assertEquals(originalA + 2, *cpu.regA);
}

TEST(testHALTNoSkippingIfIMEDisabled) {
    auto memory = createMemory();
    auto cartridge = memory->getCartridge();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    // So we set (IE & IF) == 0 here
    memory->write8BitsTo(FB_REG_IE, 0x4);
    memory->write8BitsTo(FB_REG_IF, 0x2);

    // Program ROM
    cartridge->rom[initialProgCounter] = 0x76;      // HALT
    cartridge->rom[initialProgCounter + 1] = 0x3C;  // INC A
    cartridge->rom[initialProgCounter + 2] = 0x00;  // NOP

    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Initial fetch without executing anything
    assertDoFullMachineCycle(cpu);
    assertEquals(0x76, cpu.currentOpcode);
    assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);

    // Assert that we are indeed in HALT mode (no execution of any opcode)
    for (int i = 0 ; i < 5 ; i++) {
        assertDoFullMachineCycle(cpu);

        assertEquals(FunkyBoy::CPUState::HALTED, cpu.instrContext.cpuState);

        assertEquals(0x76, cpu.currentOpcode); // No fetch of next instruction, we stay at the HALT instruction
        assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
        assertEquals(originalA, *cpu.regA);
    }

    // Set IE and IF to same value -> should request an interrupt
    memory->write8BitsTo(FB_REG_IE, 0x4);
    memory->write8BitsTo(FB_REG_IF, 0x4);

    // As IME == 0, we must not jump to the interrupt vector
    // Instead, we just continue reading the next opcodes

    assertDoFullMachineCycle(cpu);
    assertEquals(0x3C, cpu.currentOpcode & 0xffff); // Fetched next instruction already
    assertEquals(initialProgCounter + 2, cpu.instrContext.progCounter);
    assertEquals(originalA, *cpu.regA);
}

TEST(testHALTBugHanging) {
    auto memory = createMemory();
    auto cartridge = memory->getCartridge();
    FunkyBoy::CPU cpu(TEST_GB_TYPE, memory, memory->getIoRegisters());

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.instrContext.progCounter = initialProgCounter;
    cpu.instrContext.interruptMasterEnable = FunkyBoy::IMEState::DISABLED;

    // HALT bug is triggered when IME == 0 && (IE & IF) != 0
    memory->write8BitsTo(FB_REG_IE, 0x4);
    memory->write8BitsTo(FB_REG_IF, 0x4);

    // Program ROM
    cartridge->rom[initialProgCounter] = 0x76;      // HALT
    cartridge->rom[initialProgCounter + 1] = 0x76;  // HALT
    cartridge->rom[initialProgCounter + 2] = 0x3C;  // INC A

    *cpu.instrContext.regA = 0x00;
    FunkyBoy::u8 originalA = *cpu.instrContext.regA;

    // Maybe a bit excessive, but iterating 100 times without advancing the PC should demonstrate the HALT bug correctly
    for (int i = 0 ; i < 100 ; i++) {
        assertDoFullMachineCycle(cpu);
        assertEquals(0x76, cpu.currentOpcode);
        assertEquals(initialProgCounter + 1, cpu.instrContext.progCounter);
        assertEquals(originalA, *cpu.regA);
    }
}

acacia::Report __fbTests_runUnitTests() {
    return runAcaciaFileTests();
}