#include "library.h"

#include <iostream>

#include <emulator/emulator.h>
#include <cartridge/header.h>
#include <util/debug.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    FunkyBoy::Emulator emulator;
    emulator.loadGame(std::filesystem::path("funkyboy.gb"));

    unsigned long long instructions = 0;
    while (emulator.doTick()) {
        instructions++;
#ifdef FB_DEBUG_STEPS
        while (getchar() != '\n');
#endif
    }
    fprintf(stdout, "Executed %llu instructions\n", instructions);

    return 0;
}
