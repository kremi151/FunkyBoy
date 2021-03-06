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

#include "controllers.h"

#include "serial_null.h"
#include "display_void.h"

using namespace FunkyBoy::Controller;

Controllers::Controllers(
        SerialControllerPtr serial,
        DisplayControllerPtr display
): serial(std::move(serial))
, display(std::move(display))
{
}

Controllers::Controllers(): Controllers(
        std::make_shared<SerialControllerVoid>(),
        std::make_shared<DisplayControllerVoid>()
) {
}

void Controllers::setSerial(const SerialControllerPtr &serialController) {
    this->serial = serialController;
}

void Controllers::setDisplay(const DisplayControllerPtr &displayController) {
    this->display = displayController;
}
