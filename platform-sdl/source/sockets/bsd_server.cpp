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

#include "bsd_server.h"

#include "socket_exception.h"

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace FunkyBoy::SDL::Sockets;

void BSDServer::setupSocket(const CLIConfig &config) {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == 0) {
        throw SocketException("Unable to create socket");
    }
    /*int opt = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw SocketException("Unable to attach to port " + std::to_string(config.socketPort));
    }*/
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.socketPort);
    if (bind(socketFd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0) {
        throw SocketException("Unable to bind to port " + std::to_string(config.socketPort));
    }
    if (listen(socketFd, 3) < 0) {
        throw SocketException("Unable to listen on port " + std::to_string(config.socketPort));
    }
}

void BSDServer::threadMain() {
    int addrlen = sizeof(address);
    char buffer[16] = {0};
    while (socketFd > 0) {
#ifdef FB_DEBUG
        std::cout << "Accepting on socket..." << std::endl;
#endif
        int clientSocket = ::accept(socketFd, reinterpret_cast<sockaddr *>(&address),
                                    reinterpret_cast<socklen_t *>(&addrlen));
        if (clientSocket < 0) {
            continue;
        }
        std::cout << "Got connection from " << clientSocket << std::endl;

        size_t bytesRead;

        // Receive and check initial magic bytes
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead != 5 || buffer[0] != 0x42 || buffer[1] != 0x18 || buffer[2] != 0x69 || buffer[3] != 0x06 || buffer[4] != 0x55) {
            std::cerr << "Connection from " << clientSocket << " cannot be trusted" << std::endl;
            shutdown(clientSocket, 2);
            close(clientSocket);
            continue;
        }
        // Send reversed magic bytes
        buffer[4] = 0x42;
        buffer[3] = 0x18;
        buffer[2] = 0x69;
        buffer[1] = 0x06;
        buffer[0] = 0x55;
        send(clientSocket, buffer, 5, 0);

        std::cout << "Connection from " << clientSocket << " is trusted" << std::endl;

        while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
            // TODO: Transfer bit to serial controller
        }
    }
}