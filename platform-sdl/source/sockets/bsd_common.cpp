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
    , outByte(0)
    , transferring(false)
    , running(false)
{
}

BSDSocketInterface::~BSDSocketInterface() {
    if (socketFd > 0) {
#ifdef FB_DEBUG
        std::cout << "Closing socket file descriptor..." << std::endl;
#endif
        shutdown(socketFd, 2);
        close(socketFd);
        socketFd = 0;
#ifdef FB_DEBUG
        std::cout << "Socket file descriptor closed, joining input thread..." << std::endl;
#endif
        if (readThread.joinable()) {
            readThread.join();
        }
#ifdef FB_DEBUG
        std::cout << "Joining output thread..." << std::endl;
#endif
        if (sendThread.joinable()) {
            sendThread.join();
        }
#ifdef FB_DEBUG
        std::cout << "Threads have been stopped" << std::endl;
#endif
    }
}

void BSDSocketInterface::init(const CLIConfig &config) {
    setupSocket(config);
    readThread = std::thread([&] {
        readThreadMain();
    });
}

void BSDSocketInterface::handleSocketRead(int fd, char *buffer) {
    running = true;

    sendThread = std::thread([&]{
        handleSocketWrite(fd);
    });

    size_t bytesRead;
    while ((bytesRead = read(fd, buffer, 1)) > 0) {
        fprintf(stderr, "LOCK handleSocketRead\n");
        std::lock_guard<std::mutex> lock(mutex);
        fprintf(stderr, "LOCK handleSocketRead AQUIRED\n");

        std::cout << "RECEIVED INCOMING BYTE: " << (buffer[0] & 0xffff) << std::endl;
        bitReceivedCallback(buffer[0]);

        buffer[0] = outByte & 0xff;
        if (send(fd, buffer, sizeof(buffer), 0) < 1) {
#ifdef FB_DEBUG
            std::cerr << "Socket was closed while trying to send a response byte, shutting down read thread..." << std::endl;
#endif
            break;
        }
    }

    running = false;
}

void BSDSocketInterface::handleSocketWrite(int fd) {
    u8 buffer[1] = {0};
    while (running) {
        // TODO: Can we suspend the thread until an outgoing byte is available?
        if (!transferring) {
            continue;
        }
        fprintf(stderr, "LOCK handleSocketWrite\n");
        std::lock_guard<std::mutex> lock(mutex);
        fprintf(stderr, "LOCK handleSocketWrite AQUIRED\n");
        buffer[0] = outByte & 0xff;
        if (send(fd, buffer, sizeof(buffer), 0) < 1) {
#ifdef FB_DEBUG
            std::cerr << "Socket was closed while trying to send a byte, shutting down write thread..." << std::endl;
#endif
            return;
        }
        if (read(fd, buffer, sizeof(buffer)) <= 0) {
#ifdef FB_DEBUG
            std::cerr << "Socket was closed while trying to receive response byte, shutting down write thread..." << std::endl;
#endif
            return;
        }

        std::cout << "RECEIVED RESPONSE BYTE: " << (buffer[0] & 0xffff) << std::endl;
        bitReceivedCallback(buffer[0]);

        transferring = false;
    }
}

void BSDSocketInterface::setInputByte(FunkyBoy::u8_fast byte) {
    outByte = byte;
}

void BSDSocketInterface::setCallback(std::function<void(u8_fast)> callback) {
    bitReceivedCallback = callback;
}

void BSDSocketInterface::transferByte() {
    fprintf(stderr, "LOCK transferByte\n");
    std::lock_guard<std::mutex> lock(mutex);
    fprintf(stderr, "LOCK transferByte AQUIRED\n");
    transferring = true;
}