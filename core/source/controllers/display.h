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

#ifndef FB_CORE_CONTROLLERS_DISPLAY_H
#define FB_CORE_CONTROLLERS_DISPLAY_H

#include <memory>
#include <util/typedefs.h>

namespace FunkyBoy::Controller {

    class DisplayController {
    public:
        virtual ~DisplayController() = default;

        virtual void drawScanLine(u8 y, u8 *buffer) = 0;
        virtual void drawScreen() = 0;
    };

    typedef std::shared_ptr<DisplayController> DisplayControllerPtr;

}

#endif //FB_CORE_CONTROLLERS_DISPLAY_H
