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

#include <unistd.h>
#include <iostream>

using namespace FunkyBoy::SDL::Sockets;

BSDServer::BSDServer(const CLIConfig &config) {
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == 0) {
        throw SocketException("Unable to create socket");
    }
    /*int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw SocketException("Unable to attach to port " + std::to_string(config.socketPort));
    }*/
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.socketPort);
    if (bind(serverFd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0) {
        throw SocketException("Unable to bind to port " + std::to_string(config.socketPort));
    }
    if (listen(serverFd, 3) < 0) {
        throw SocketException("Unable to listen on port " + std::to_string(config.socketPort));
    }
    serverThread = std::thread([&] {
        accept();
    });
}

BSDServer::~BSDServer() {
#ifdef FB_DEBUG
    std::cout << "Closing socket file descriptor..." << std::endl;
#endif
    shutdown(serverFd, 2);
    close(serverFd);
    serverFd = 0;
#ifdef FB_DEBUG
    std::cout << "Socket file descriptor closed, joining server thread..." << std::endl;
#endif
    serverThread.join();
#ifdef FB_DEBUG
    std::cout << "Server thread has stopped" << std::endl;
#endif
}

void BSDServer::accept() {
    int addrlen = sizeof(address);
    char buffer[16] = {0}; // TODO: Large enough?
    while (serverFd) {
#ifdef FB_DEBUG
        std::cout << "Accepting on socket..." << std::endl;
#endif
        int clientSocket = ::accept(serverFd, reinterpret_cast<sockaddr *>(&address),
                                    reinterpret_cast<socklen_t *>(&addrlen));
        if (clientSocket < 0) {
            continue;
        }
        std::cout << "Got connection from " << clientSocket << std::endl;

        size_t bytesRead;
        while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
            // TODO: Transfer bit to serial controller
        }
    }
}