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

#include "controllers_test.h"
#include <iostream>

namespace FunkyBoy::Controllers::Test {
    extern char lastWord[FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE + 1];
}

using namespace FunkyBoy;

void Controllers::init() {}
void Controllers::destroy() {}

using namespace FunkyBoy::Controllers;

void Display::drawScanLine(FunkyBoy::u8 y, const FunkyBoy::u8 *buffer) {}
void Display::drawScreen() {}

bool Joypad::isKeyPressed(FunkyBoy::Controllers::Joypad::JoypadKey key) {
    return false;
}

void Serial::sendByte(FunkyBoy::u8 data) {
    std::cout << data;
    for (u8 i = 1 ; i < FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE ; i++) {
        Test::lastWord[i - 1] = Test::lastWord[i];
    }
    Test::lastWord[FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE - 1] = data;
}