/**
 * Copyright 2021 Michel Kremer (kremi151)
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

#ifndef FB_SDL_SOCKETS_BSD_COMMON_H
#define FB_SDL_SOCKETS_BSD_COMMON_H

#include "socket_interface.h"

#include <thread>
#include <mutex>
#include <netinet/in.h>

namespace FunkyBoy::SDL::Sockets {

    class BSDSocketInterface : public SocketInterface {
    private:
        bool running;
        std::thread sendThread, readThread;
        std::function<void(u8_fast)> bitReceivedCallback;
    protected:
        std::mutex mutex;
        int socketFd;
        struct sockaddr_in address{};

        bool transferring;
        u8_fast outByte;

        void handleSocketRead(int fd, char *buffer);
        void handleSocketWrite(int fd);

        virtual void readThreadMain() = 0;
        virtual void setupSocket(const CLIConfig &config) = 0;

    public:
        BSDSocketInterface();
        ~BSDSocketInterface() override;

        void init(const CLIConfig &config) final;
        void setInputByte(FunkyBoy::u8_fast byte) final;
        void setCallback(std::function<void(u8_fast)> bitReceivedCallback) final;
        void transferByte() final;
    };

}

#endif //FB_SDL_SOCKETS_BSD_COMMON_H
