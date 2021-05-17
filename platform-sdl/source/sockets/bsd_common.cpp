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

#include "bsd_common.h"

#include <unistd.h>
#include <iostream>

using namespace FunkyBoy::SDL::Sockets;

BSDSocketInterface::BSDSocketInterface()
    : socketFd(0)
{
}

BSDSocketInterface::~BSDSocketInterface() {
    if (thread.joinable()) {
#ifdef FB_DEBUG
        std::cout << "Closing socket file descriptor..." << std::endl;
#endif
        shutdown(socketFd, 2);
        close(socketFd);
        socketFd = 0;
#ifdef FB_DEBUG
        std::cout << "Socket file descriptor closed, joining server thread..." << std::endl;
#endif
        thread.join();
#ifdef FB_DEBUG
        std::cout << "Server thread has stopped" << std::endl;
#endif
    }
}

void BSDSocketInterface::init(const CLIConfig &config) {
    setupSocket(config);
    thread = std::thread([&] {
        threadMain();
    });
}

void BSDSocketInterface::transferBit(FunkyBoy::u8_fast bit, std::function<void(u8_fast)> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: queue.push(bit);
}