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

#include "commons.h"
#include "controllers/serial_test.h"

#include <acacia.h>
#include <memory/memory.h>
#include <emulator/emulator.h>

void testUsingROM(const std::filesystem::path &romPath, unsigned int expectedTicks) {
    expectedTicks *= 4;
    auto serial = std::make_shared<FunkyBoy::Controller::SerialControllerTest>();
    FunkyBoy::Emulator emulator(
            TEST_GB_TYPE,
            serial
    );
    auto status = emulator.loadGame(romPath);
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    for (unsigned int i = 0 ; i < expectedTicks ; i++) {
        if (!emulator.doTick()) {
            testFailure("Emulation tick failed");
        }
        if (std::strcmp("Passed", serial->lastWord) == 0) {
            std::cout << std::endl;
            break;
        }
    }

    // Blargg's test ROMs will print "Passed" if the tests have passed and "Failed" otherwise
    assertStandardOutputHasNot("Failed");
    assertStandardOutputHas("Passed");
}
