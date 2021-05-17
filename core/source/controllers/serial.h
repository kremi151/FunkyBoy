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

#ifndef FB_CORE_CONTROLLERS_SERIAL_H
#define FB_CORE_CONTROLLERS_SERIAL_H

#include <memory>
#include <util/typedefs.h>
#include <functional>

namespace FunkyBoy::Controller {

    class SerialController {
    public:
        virtual ~SerialController() = default;

        virtual void setup(std::function<void(u8_fast)> bitReceived) = 0;
        virtual void sendBit(u8 data) = 0;
    };

    typedef std::shared_ptr<SerialController> SerialControllerPtr;

}

#endif //FB_CORE_CONTROLLERS_SERIAL_H
