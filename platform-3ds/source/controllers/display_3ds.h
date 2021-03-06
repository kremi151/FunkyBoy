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

#ifndef FB_3DS_CONTROLLERS_DISPLAY_3DS_H
#define FB_3DS_CONTROLLERS_DISPLAY_3DS_H

#include <controllers/display.h>

namespace FunkyBoy::Controller {

    class DisplayController3DS: public DisplayController {
    private:
        u8 *frameBuffer;
        u16 frameWidth, frameHeight;

        u8 offsetEffectiveX, offsetEffectiveY;
        bool offsetsCalculated;
    public:
        DisplayController3DS();

        void drawScanLine(u8 y, u8 *buffer) override;
        void drawScreen() override;
    };

}

#endif //FB_3DS_CONTROLLERS_DISPLAY_3DS_H
