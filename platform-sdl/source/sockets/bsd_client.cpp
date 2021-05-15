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

#include "bsd_client.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "socket_exception.h"

using namespace FunkyBoy::SDL::Sockets;

BSDClient::BSDClient(const CLIConfig &config) {
    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd < 0) {
        throw SocketException("Unable to create socket");
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(config.socketPort);
    // TODO: Support domains
    if (inet_pton(AF_INET, config.socketAddress, &serverAddress.sin_addr) <= 0) {
        throw SocketException("Invalid address or address not supported: " + std::string(config.socketAddress));
    }
    if (connect(clientFd, reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        throw SocketException("Connection failed");
    }

    char buffer[8] = {0};
    size_t bytesRead;

    // Send some magic bytes to initiate connection
    buffer[0] = 0x42;
    buffer[1] = 0x18;
    buffer[2] = 0x69;
    buffer[3] = 0x06;
    buffer[4] = 0x55;
    send(clientFd, buffer, 5, 0);

    bytesRead = read(clientFd, buffer, sizeof(buffer));
    if (bytesRead != 5 || buffer[4] != 0x42 || buffer[3] != 0x18 || buffer[2] != 0x69 || buffer[1] != 0x06 || buffer[0] != 0x55) {
        throw SocketException("Connection cannot be trusted");
    }

    std::cout << "Connection is trusted" << std::endl;

    clientThread = std::thread([&] {
        run();
    });
}

BSDClient::~BSDClient() {
#ifdef FB_DEBUG
    std::cout << "Closing socket file descriptor..." << std::endl;
#endif
    shutdown(clientFd, 2);
    close(clientFd);
    clientFd = 0;
#ifdef FB_DEBUG
    std::cout << "Socket file descriptor closed, joining client thread..." << std::endl;
#endif
    clientThread.join();
#ifdef FB_DEBUG
    std::cout << "Client thread has stopped" << std::endl;
#endif
}

void BSDClient::run() {
    char buffer[16] = {0};
    size_t bytesRead;
    while (clientFd > 0) {
        // TODO: Handle serial bits
    }
}