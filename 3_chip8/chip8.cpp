#include "chip8.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <format>

const std::uint8_t Chip8::fontset[FONTSET_SIZE] = {
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

Chip8::Chip8() {
    load_fontset();
}

void Chip8::load_fontset() {
    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

bool Chip8::load_rom(const std::string& file_path)
{
    // open file in binary mode and position on the end of the binary stream
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) 
    {
        std::cerr << "Failed to open ROM!" << std::endl;
        return false;
    }

    // because we are in the end of the stream, use tellg to return the position (which represents the size)
    std::streamsize size = file.tellg();
    // return to the beginning of the file to start to read
    file.seekg(0, std::ios::beg);

    // load the entire rom on the buffer
    char* buffer = new char[size];
    file.read(buffer, size);
    file.close();

    // write from buffer to memory
    for (int i = 0; i < size; i++) 
    {
        if ((ROM_START_ADDRESS + i) < MEMORY_SIZE) 
        {
            memory[ROM_START_ADDRESS + i] = (uint8_t)buffer[i];
        }
        else 
        {
            std::cerr << "ROM size is bigger than the memory size!" << std::endl;
            return false;
        }
    }

    return true;
}

std::uint16_t Chip8::fetch()
{
    std::uint16_t first_half_opcode = (std::uint16_t) memory[program_counter] << 8;
    std::uint16_t second_half_opcode = (std::uint16_t) memory[program_counter+1];
    std::uint16_t full_opcode = first_half_opcode + second_half_opcode;
    // std::cout << "Instruction: 0x" 
    //           << std::uppercase << std::hex << std::setw(4) << std::setfill('0') 
    //           << full_opcode << std::endl;
    
    program_counter += 2;

    //DEBUG
    set_opcode(full_opcode);

    return full_opcode;
}

// decode and execute
void Chip8::decode(std::uint16_t opcode)
{
    std::uint8_t type = (std::uint8_t) (opcode >> 12);
    std::uint8_t X = (std::uint8_t) (opcode >> 8) & 0x0F;
    std::uint8_t Y = (std::uint8_t) (opcode >> 4) & 0x0F;
    std::uint8_t N = (std::uint8_t) (opcode) & 0x0F;
    std::uint8_t NN = (std::uint8_t) opcode;
    std::uint16_t NNN = opcode & 0x0FFF;

    switch (type)
    {
        case 0x00:
            // 00E0: CLEAR SCREEN
            // std::cout << "00E0: CLEAR SCREEN" << std::endl;
            if (NNN == 0x00e0)
            {
                for (int i = 0; i < DISPLAY_WIDTH; i++){
                    display[i].fill(0);
                }
            }
            break;
        case 0x01:
            // 1NNN: JUMP
            // std::cout << "1NNN: JUMP" << std::endl;
            program_counter = NNN;
            break;
        case 0x06:
            // 6XNN: SET REGISTER
            // std::cout << "6XNN: SET REGISTER" << std::endl;
            V[X] = NN;
            break;
        case 0x07:
            // 7XNN: ADD REGISTER
            // std::cout << "7XNN: ADD REGISTER" << std::endl;
            V[X] += NN; 
            break;
        case 0x0A:
            // ANNN: SET INDEX REGISTER
            // std::cout << "ANNN: SET INDEX REGISTER" << std::endl;
            I = NNN;
            break;

        case 0x0D:
            // DXYN: DRAW 
            // std::cout << "DXYN: DRAW " << std::endl;
            std::uint8_t X_coord = V[X] % DISPLAY_WIDTH;
            std::uint8_t Y_coord = V[Y] % DISPLAY_HEIGHT;
            std::uint8_t mask = 1; 
            V[REGISTER_SIZE - 1] = 0;
            for (int i = 0; i < N; i++){
                printf("X_coord = %d, Y_coord = %d\n", (int)X_coord, (int)Y_coord);
                std::uint8_t sprite_row = memory[I + i];
                printf("memory[%d] = 0x%02X\n", I + i, sprite_row);
                for (int j = 0; j < 8; j++){
                    if (X_coord >= DISPLAY_WIDTH)
                    {
                        continue;
                    }
                    std::uint8_t current_pixel = sprite_row >> (7 - j);
                    
                    // take only the first bit (from right)
                    current_pixel = current_pixel & mask;

                    if (current_pixel == 1 && display[X_coord][Y_coord] == 1)
                    {
                        display[X_coord][Y_coord] = 0;
                        V[REGISTER_SIZE - 1] = 1;
                    }
                    else if (current_pixel == 1 && display[X_coord][Y_coord] == 0)
                    {
                        display[X_coord][Y_coord] = 1;
                    }
                    X_coord++;
                }
                X_coord = V[X] % DISPLAY_WIDTH;
                
                if (Y_coord >= DISPLAY_HEIGHT)
                {
                    continue;
                }
                Y_coord++;
            }
            break;
    }
}

void Chip8::cycle()
{
    std::uint16_t opcode = fetch();
    decode(opcode);
}