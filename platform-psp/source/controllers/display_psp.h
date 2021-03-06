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

#ifndef FB_PSP_CONTROLLERS_DISPLAY_PSP_H
#define FB_PSP_CONTROLLERS_DISPLAY_PSP_H

#include <controllers/display.h>

namespace FunkyBoyPSP::Controller {

    class DisplayController: public FunkyBoy::Controller::DisplayController {
    public:
        DisplayController();

        uint_fast8_t offsetX, offsetY;

        uint32_t *frameBuffer{};

        void drawScanLine(FunkyBoy::u8 y, FunkyBoy::u8 *buffer) override;
        void drawScreen() override;
    };

}

#endif //FB_PSP_CONTROLLERS_DISPLAY_PSP_H
