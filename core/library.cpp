#include "library.h"

#include <iostream>

#include <emulator/emulator.h>
#include <cartridge/header.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    FunkyBoy::Emulator emulator;
    emulator.loadGame(std::filesystem::path("funkyboy.gb"));
    while (emulator.doTick());

    return 0;
}
