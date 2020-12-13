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

#include "rom_commons.h"
#include "../controllers/controllers_test.h"

#include <acacia.h>
#include <memory/memory.h>
#include <emulator/emulator.h>
#include <cstring>

namespace FunkyBoy::Controllers::Test {
    char lastWord[FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE + 1]{};
}

void testUsingROM(const FunkyBoy::fs::path &romPath, unsigned int expectedTicks, const char *successWord, const char *failureWord) {
    // Clear last word
    for (int i = 0 ; i < FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE ; i++) {
        FunkyBoy::Controllers::Test::lastWord[i] = '\0';
    }

    expectedTicks *= 4;
    FunkyBoy::Emulator emulator(TEST_GB_TYPE);
    auto status = emulator.loadGame(romPath);
    if (status != FunkyBoy::CartridgeStatus::Loaded) {
      std::cout << "Loading test ROM at " << romPath << " failed" << std::endl;
    }
    assertEquals(FunkyBoy::CartridgeStatus::Loaded, status);

    for (unsigned int i = 0 ; i < expectedTicks ; i++) {
        if (!emulator.doTick()) {
            testFailure("Emulation tick failed");
        }
        if (std::strcmp(successWord, FunkyBoy::Controllers::Test::lastWord) == 0) {
            std::cout << std::endl;
            break;
        } else if (std::strcmp(failureWord, FunkyBoy::Controllers::Test::lastWord) == 0) {
            testFailure("Test has failed");
            break;
        }
    }

    // Blargg's test ROMs will print "Passed" if the tests have passed and "Failed" otherwise
    // Mooneye ROMs will output some magic number sequences depending of the success
    assertStandardOutputHasNot(failureWord);
    assertStandardOutputHas(successWord);
}
