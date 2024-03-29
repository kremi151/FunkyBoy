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

#ifndef FB_CORE_CONTROLLERS_CONTROLLERS_H
#define FB_CORE_CONTROLLERS_CONTROLLERS_H

#include "serial.h"
#include "joypad.h"
#include "display.h"
#include "audio.h"

#include <util/typedefs.h>

#include <memory>

namespace FunkyBoy::Controller {

    class Controllers {
    private:
        const SerialControllerPtr serial;
        const DisplayControllerPtr display;
        const AudioControllerPtr audio;

    public:
        Controllers(SerialControllerPtr serial, DisplayControllerPtr display, AudioControllerPtr audio);
        Controllers();

        inline const SerialControllerPtr &getSerial() const {
            return serial;
        }

        inline const DisplayControllerPtr &getDisplay() const {
            return display;
        }

        inline const AudioControllerPtr &getAudio() const {
            return audio;
        }

        Controllers withSerial(SerialControllerPtr serial);
        Controllers withDisplay(DisplayControllerPtr display);
        Controllers withAudio(AudioControllerPtr audio);
    };

    typedef std::shared_ptr<Controllers> ControllersPtr;

}

#endif //FB_CORE_CONTROLLERS_CONTROLLERS_H
