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

#include <util/typedefs.h>

#include <memory>

namespace FunkyBoy::Controller {

    class Controllers {
    private:
        SerialControllerPtr serial;
        DisplayControllerPtr display;

    public:
        Controllers(SerialControllerPtr serial, DisplayControllerPtr display);
        Controllers();

        inline SerialControllerPtr &getSerial() {
            return serial;
        }

        void setSerial(const SerialControllerPtr &serialController);

        inline DisplayControllerPtr &getDisplay() {
            return display;
        }

        void setDisplay(const DisplayControllerPtr &displayController);
    };

    typedef std::shared_ptr<Controllers> ControllersPtr;

}

#endif //FB_CORE_CONTROLLERS_CONTROLLERS_H
