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

void BSDClient::setupSocket(const CLIConfig &config) {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        throw SocketException("Unable to create socket");
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(config.socketPort);
    // TODO: Support domains
    if (inet_pton(AF_INET, config.socketAddress, &address.sin_addr) <= 0) {
        throw SocketException("Invalid address or address not supported: " + std::string(config.socketAddress));
    }
    if (connect(socketFd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0) {
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
    send(socketFd, buffer, 5, 0);

    bytesRead = read(socketFd, buffer, sizeof(buffer));
    if (bytesRead != 5 || buffer[4] != 0x42 || buffer[3] != 0x18 || buffer[2] != 0x69 || buffer[1] != 0x06 || buffer[0] != 0x55) {
        throw SocketException("Connection cannot be trusted");
    }

    std::cout << "Connection is trusted" << std::endl;
}

void BSDClient::readThreadMain() {
    u8 buffer[16] = {0};
    handleSocketRead(socketFd, buffer, sizeof(buffer));
}