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

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    

    Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
    {
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
        //Load fontset into memory.
        for (unsigned int i = 0; i < 80; i++){
            memory[FONTSET_START + i] = fontset[i];
        }

        //Seed RNG
        randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
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

    //Instructions for Chip-8 begin here.

    //Clear display (CLS)
    void Chip8::OP_00E0(){
        memset(video, 0, sizeof(video));
    }

    //Return from a subroutine(RET)
    void Chip8::OP_00EE(){
         sp--;
         pc = stack[sp];
    }

    //Jump to a location (nnn)
    void Chip8::OP_1nnn(){
        uint16_t address = opcode & 0x0FFFu;
        pc = address;
    }

    //Call subroutine at nnn
    void Chip8::OP_2nnn(){
        uint16_t address = opcode & 0x0FFFu;
        stack[sp] = pc;
        sp++;
        pc = address;
    }

    //Skip next instruction if Vx == kk
    void Chip8::OP_3xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x0FFu;

        if(registers[Vx] == byte){
            pc += 2;
        }
    }

    //Skip next instruction if Vx != kk
    void Chip8::OP_4xkk(){
        uint8_t Vx = (opcode &0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFu;

        if(registers[Vx] != byte){
            pc += 2;
        }
    }

    //Skip next instruction if Vx == Vy
    void Chip8::OP_5xy0(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if(registers[Vx] == registers[Vy]){
            pc += 2;
        }
    }

    //Set Vx to kk
    void Chip8::OP_6xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFu;

        registers[Vx] = byte;
    }

    //Add Vx and kk values

    void Chip8::OP_7xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFu;

        registers[Vx] += byte;
    }

    //Sets Vx to Vy
    void Chip8::OP_8xy0(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vy];
    }

    //Set Vx or Vy
    void Chip8::OP_8xy1(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] |= registers[Vy];
    }

    //Set Vx to Vx AND Vy
    void Chip8::OP_8xy2(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] &= registers[Vy];
    }

    //Set VX = Vx XOR Vy
    void Chip8::OP_8xy3(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] ^= registers[Vy];
    }

    //Add Vx and Vy, with carry value Vf
    void Chip8::OP_8xy4(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;
        uint16_t sum = registers[Vx] + registers[Vy];

        if (sum > 255U){
            registers[0xF] = 1;
        }
        else{
            registers[0xF] = 0;
        }

        registers[Vx] = sum & 0xFFu;    
    }

    //Subtract Vx and Vy, Vf set to not borrow.
    void Chip8::OP_8xy5(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if(registers[Vx] > registers[Vy]) {
            registers[0xF] = 1;
        }
        else {
            registers[0xF] = 0;
        }

        registers[Vx] -= registers[Vy];
    }

    //If the least sig bit of Vx is 1, Vf gets set to 1. Vx is divided by 2.
    void Chip8::OP_8xy6(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        registers[0xF] = (registers[Vx] & 0x1u);
        registers[Vx] >>= 1;
    }

    



};