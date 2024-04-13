#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

class Chip8 {

    //Various components of the Chip8 system.
    public:
        uint8_t registers[16] {};
        uint8_t memory[4096] {};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64 * 32]{};
        uint16_t opcode;

    const unsigned int START_ADDRESS = 0x200;
    const unsigned int FONTSET_START = 0x50;
    const unsigned int FONTSET_SIZE = 80;
    

    Chip8::Chip8(){
        //Things here need to happen first.
        pc = START_ADDRESS;

        uint8_t fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

        for (unsigned int i = 0; i < 80; i++){
            memory[FONTSET_START + i] = fontset[i];
        }
    }

    void Chip8::LoadROM(char const* filename){

        //Open a file as a stream, file pointer goes to end.
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if(file.is_open()){
            std::streampos size= file.tellg();
            char* buffer = new char[size];

            //Go back to the beginning of the file, fill buffer.
            file.seekg(0,std::ios::beg);
            file.read(buffer,size);
            file.close();

            //Load ROM into memory.
            for (long i = 0; i < size; i++){
                memory[START_ADDRESS + i] = buffer[i];
            }

            delete[] buffer;
        }
    }

};