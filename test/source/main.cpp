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

#include <acacia.h>
#include <memory/memory.h>
#include <memory>
#include <emulator/emulator.h>
#include <fstream>

#define RUN_ROM_TESTS

TEST(test16BitReadWrite) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    FunkyBoy::Memory memory(cartridge);

    memory.write16BitsTo(0xC000, 0x12, 0x34);
    int val = memory.read16BitsAt(0xC000);
    assertEquals(0x1234, val);

    memory.write16BitsTo(0xC002, 0x1234);
    val = memory.read16BitsAt(0xC002);
    assertEquals(0x1234, val);
}

TEST(testEchoRAM) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    FunkyBoy::Memory memory(cartridge);

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
    FunkyBoy::Emulator emulator;
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "cpu_instrs.gb";
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    auto &cartridge = emulator.getCartridge();
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, cartridge.getStatus());
    assertEquals("CPU_INSTRS", std::string(reinterpret_cast<const char*>(cartridge.getHeader()->title)));
}

TEST(testPopPushStackPointer) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge);
    FunkyBoy::CPU cpu(memory);

    cpu.push16Bits(0x1806);
    auto val = cpu.pop16Bits();
    assertEquals(0x1806, val);

    cpu.push16Bits(0x28, 0x09);
    cpu.push16Bits(0x1223);
    cpu.push16Bits(0x42, 0x69);

    val = cpu.pop16Bits();
    assertEquals(0x4269, val);
    val = cpu.pop16Bits();
    assertEquals(0x1223, val);
    val = cpu.pop16Bits();
    assertEquals(0x2809, val);
}

TEST(testReadWriteHLAndAF) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge);
    FunkyBoy::CPU cpu(memory);

    // In this test, we check for enforcing little-endianness

    cpu.writeAF(0x1234);
    // 0x34 -> 0b110100
    // 0x34 -> F -> 0b110000 -> 0x30 (register F only stores the 4 most significant bits)
    assertEquals(0x30, *cpu.regF_do_not_use_directly);
    assertEquals(0x12, *cpu.regA);
    auto val = cpu.readAF();
    assertEquals(0x1230, val);

    cpu.writeHL(0x1806);
    assertEquals(0x06, *cpu.regL);
    assertEquals(0x18, *cpu.regH);
    val = cpu.readHL();
    assertEquals(0x1806, val);
}

TEST(testReadWrite16BitRegisters) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge);
    FunkyBoy::CPU cpu(memory);

    // In this test, we check for enforcing little-endianness

    // BC
    cpu.write16BitRegister(0, 0x1234);
    assertEquals(0x34, *cpu.regC);
    assertEquals(0x12, *cpu.regB);
    auto val = cpu.read16BitRegister(0);
    assertEquals(0x1234, val);

    // DE
    cpu.write16BitRegister(1, 0x1806);
    assertEquals(0x06, *cpu.regE);
    assertEquals(0x18, *cpu.regD);
    val = cpu.read16BitRegister(1);
    assertEquals(0x1806, val);

    // HL
    cpu.write16BitRegister(2, 0x2809);
    assertEquals(0x09, *cpu.regL);
    assertEquals(0x28, *cpu.regH);
    val = cpu.read16BitRegister(2);
    assertEquals(0x2809, val);
}

TEST(test16BitLoads) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge);
    FunkyBoy::CPU cpu(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter + 1] = 0x06;
    cartridge->rom[initialProgCounter + 2] = 0x18;

    // Set opcode 0x01 (LD BC,d16)
    cartridge->rom[initialProgCounter] = 0x01;
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 3, cpu.progCounter);
    assertEquals(0x06, (*cpu.regC) & 0xff);
    assertEquals(0x18, (*cpu.regB) & 0xff);

    // Set opcode 0x11 (LD DE,d16)
    cpu.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x11;
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 3, cpu.progCounter);
    assertEquals(0x06, (*cpu.regE) & 0xff);
    assertEquals(0x18, (*cpu.regD) & 0xff);

    // Set opcode 0x21 (LD HL,d16)
    cpu.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x21;
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 3, cpu.progCounter);
    assertEquals(0x06, (*cpu.regL) & 0xff);
    assertEquals(0x18, (*cpu.regH) & 0xff);

    // Set opcode 0x31 (LD SP,d16)
    cpu.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter] = 0x31;
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 3, cpu.progCounter);
    assertEquals(0x1806, cpu.stackPointer);
}

TEST(testLDHA) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    auto memory = std::make_shared<FunkyBoy::Memory>(cartridge);
    FunkyBoy::CPU cpu(memory);

    // Allocate a simulated ROM, will be destroyed by the cartridge's destructor
    cartridge->rom = new FunkyBoy::u8[0x105];

    auto initialProgCounter = 0x100;
    cpu.progCounter = initialProgCounter;
    cartridge->rom[initialProgCounter + 1] = 0xCE;

    // Set opcode 0xF0 (LDH A,(a8))
    cartridge->rom[initialProgCounter] = 0xF0;
    memory->write8BitsTo(0xFFCE, 0x42);
    assertNotEquals(0x42, *cpu.regA);
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 2, cpu.progCounter);
    assertEquals(0x42, *cpu.regA);

    // Reset prog counter and register
    cpu.progCounter = initialProgCounter;
    memory->write8BitsTo(0xFFCE, 0x0);

    // Set opcode 0xE0 (LDH (a8),A)
    cartridge->rom[initialProgCounter] = 0xE0;
    *cpu.regA = 0x42;
    assertNotEquals(0x42, memory->read8BitsAt(0xFFCE));
    if (!cpu.doTick()) {
        failure("Emulation tick failed");
    }
    assertEquals(initialProgCounter + 2, cpu.progCounter);
    assertEquals(0x42, memory->read8BitsAt(0xFFCE));
}

#ifdef RUN_ROM_TESTS

void testUsingROM(const std::filesystem::path &romPath, unsigned int expectedTicks) {
    FunkyBoy::Emulator emulator;
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    for (unsigned int i = 0 ; i < expectedTicks ; i++) {
        if (!emulator.doTick()) {
            failure("Emulation tick failed");
        }
    }

    // Blargg's test ROMs will print "Passed" if the tests have passed
    assertStandardOutputHas("Passed");
}

// TODO: 01-special.gb

// TODO: 02-interrupts.gb

TEST(testCPUInstructionsOpSPHL) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "03-op sp,hl.gb";
    testUsingROM(romPath, 1120000);
}

// TODO: 04-op r,imm.gb

TEST(testCPUInstructionsOpRP) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "05-op rp.gb";
    testUsingROM(romPath, 1810000);
}

TEST(testCPUInstructionsLoads) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "06-ld r,r.gb";
    testUsingROM(romPath, 304455);
}

TEST(testCPUInstructionsJrJpCallRetRst) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "07-jr,jp,call,ret,rst.gb";
    testUsingROM(romPath, 342773);
}

TEST(testCPUInstructionsMisc) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "08-misc instrs.gb";
    testUsingROM(romPath, 285074);
}

TEST(testCPUInstructionsOpRR) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "09-op r,r.gb";
    testUsingROM(romPath, 4550740);
}

TEST(testCPUInstructionsBitOps) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "10-bit ops.gb";
    testUsingROM(romPath, 8550740);
}

TEST(testCPUInstructionsOpAHL) {
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "11-op a,(hl).gb";
    testUsingROM(romPath, 8550740);
}

#endif

int main() {
    auto report = runAcaciaTests();

    std::ofstream reportFile("test-report.xml");
    acacia::generateJUnitReport(report, reportFile);

    return report ? 0 : 1;
}