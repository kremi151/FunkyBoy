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

TEST(testCPUInstructionsJrJpCallRetRst) {
    FunkyBoy::Emulator emulator;
    std::filesystem::path romPath = std::filesystem::path("..") / "gb-test-roms" / "cpu_instrs" / "individual" / "07-jr,jp,call,ret,rst.gb";
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    for (unsigned long long i = 0 ; i < 10000000000 ; i++) {
        if (!emulator.doTick()) {
            failure("Emulation tick failed");
        }
    }

    // TODO: Check if test delivered correct result
    failure("TODO: Check if test delivered correct result");
}

int main() {
    auto report = runAcaciaTests();

    std::ofstream reportFile("test-report.xml");
    acacia::generateJUnitReport(report, reportFile);

    return report ? 0 : 1;
}