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

#ifndef FB_TESTS_CONTROLLERS_SERIAL_TEST_H
#define FB_TESTS_CONTROLLERS_SERIAL_TEST_H

#include <controllers/serial.h>

// sizeof("Passed") = 6
#define FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE 6

namespace FunkyBoy::Controller {

    class SerialControllerTest: public SerialController {
    private:
        u8 outByte{};
    public:
        void setup(std::function<void(u8_fast)> bitReceived) override;
        void transferByte() override;
        void setByte(u8_fast byte) override;

        char lastWord[FB_TEST_SERIAL_CONTROLLER_LWORD_SIZE + 1]{};
    };

}

#endif //FB_TESTS_CONTROLLERS_SERIAL_TEST_H
