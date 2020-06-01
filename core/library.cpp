#include "library.h"

#include <iostream>
#include <chrono>

#include <emulator/emulator.h>
#include <util/debug.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    FunkyBoy::Emulator emulator;
    emulator.loadGame(std::filesystem::path("funkyboy.gb"));

    unsigned long long instructions = 0;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    while (emulator.doTick()) {
        instructions++;
#ifdef FB_DEBUG_STEPS
        while (getchar() != '\n');
#endif
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    fprintf(stdout, "Executed %llu instructions\n", instructions);

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

    return 0;
}
