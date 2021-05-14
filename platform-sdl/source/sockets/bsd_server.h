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

#ifndef FB_SDL_SOCKETS_BSD_SERVER_H
#define FB_SDL_SOCKETS_BSD_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <memory>

#include <cli/config.h>

namespace FunkyBoy::SDL::Sockets {

    class BSDServer {
    private:
        int serverFd;
        struct sockaddr_in address{};
        std::thread serverThread;

        void accept();
    public:
        explicit BSDServer(const CLIConfig &config);
        ~BSDServer();
    };

    typedef std::unique_ptr<BSDServer> BSDServerPtr;

}

#endif //FB_SDL_SOCKETS_BSD_SERVER_H
