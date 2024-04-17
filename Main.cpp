/* 
    This program is a personal project to learn more about machine emulation. 
    A lot of people say that CHIP-8 is a good beginner system so I have done this to learn more and maybe continue on this path for more complex emulation.
*/


#include <chrono>
#include <iostream>
#include <string>
#include "Chip8.hpp"
#include "Platform.hpp"

//Width: 64, Height: 32

int main (int argc, char* argv[]){
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi (argv[1]);
    int cycleDelay = std::stoi (argv[2]);
    char const* romFilename = argv[3];

    Platform platform("CHIP-8 Emulator", 64 * videoScale, 32 * videoScale, 64, 32);
    Chip8 chip8;
    chip8.LoadROM(romFilename);
    int videoPitch = sizeof(chip8.video[0]) * 64;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit){
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }
    return 0;

}; 