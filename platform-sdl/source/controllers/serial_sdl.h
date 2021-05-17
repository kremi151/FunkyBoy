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

#ifndef FB_SDL_CONTROLLERS_SERIAL_SDL_H
#define FB_SDL_CONTROLLERS_SERIAL_SDL_H

#include <controllers/serial.h>

#if FB_HAS_SOCKETS
#include <sockets/socket_interface.h>
#endif

namespace FunkyBoy::Controller {

    class SerialControllerSDL: public SerialController {
    private:
        std::function<void(u8_fast)> bitReceived;

#if FB_HAS_SOCKETS
        SDL::Sockets::SocketInterfacePtr socketInterface;
#endif
    public:
#if FB_HAS_SOCKETS
        explicit SerialControllerSDL(SDL::Sockets::SocketInterfacePtr socketInterface);
#endif

        void setup(std::function<void(u8_fast)> bitReceived) override;
        void sendBit(u8 data) override;
    };

}

#endif //FB_SDL_CONTROLLERS_SERIAL_SDL_H
