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
    std::uint16_t first_half_instruction = memory[program_counter] << 8;
    std::uint16_t second_half_instruction = memory[program_counter+1];
    std::uint16_t full_instruction = first_half_instruction + second_half_instruction;
    program_counter += 2;
    return full_instruction;
}

void Chip8::decode(std::uint16_t instruction)
{
    
}