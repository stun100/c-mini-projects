#include "chip8.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include <random>

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

Chip8::Chip8(bool debug_mode) {
    is_debug = debug_mode;
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
            if (NNN == 0x00e0)
            {
                OP_00E0();
            } 
            else if (NNN == 0x00ee)
            {
               OP_00E0();
            }
            break;
        case 0x01:
            OP_1NNN(NNN);
            break;
        case 0x02:
            OP_2NNN(NNN);
            break;
        case 0x03:
            OP_3XNN(X, NN);
            break;
        case 0x04:
            OP_4XNN(X, NN);
            break;
        case 0x05:
            OP_5XY0(X,Y);
            break;
        case 0x06:
            OP_6XNN(X,NN);
            break;
        case 0x07:
            OP_7XNN(X, NN);
            break;
        case 0x08:
            OP_8XYN(X, Y, N);
            break;
        case 0x09:
            OP_9XY0(X,Y);
            break;

        case 0x0A:
            OP_ANNN(NNN);
            break;

        case 0x0B:
            OP_BNNN(NNN);
            break;
        
        case 0x0C:
            OP_CXNN(X, NN);
            break;

        case 0x0D:
            OP_DXYN(X,Y,N);
            break;

        // case 0x0E:
        //     // TODO
        //     break;

    }
}

void Chip8::cycle()
{
    std::uint16_t opcode = fetch();
    decode(opcode);
}

void Chip8::OP_00E0()
{
    if (is_debug) std::cout << "00E0: CLEAR SCREEN" << std::endl;
    for (int i = 0; i < DISPLAY_WIDTH; i++){
        display[i].fill(0);
    }
}

void Chip8::OP_OOEE()
{
    if (is_debug) std::cout << "00EE: RETURN FROM SUBROUTINE" << std::endl;
    program_counter = st.top();
    st.pop();
}

void Chip8::OP_1NNN(std::uint16_t NNN)
{
    if (is_debug) std::cout << "1NNN: JUMP TO " << std::hex << NNN << std::dec << std::endl;
    program_counter = NNN;
}

void Chip8::OP_2NNN(std::uint16_t NNN)
{
    if (is_debug) std::cout << "2NNN: CALL SUBROUTINE AT " << std::hex << NNN << std::dec << std::endl;
    st.push(program_counter);
    program_counter = NNN; 
}

void Chip8::OP_3XNN(std::uint8_t X, std::uint8_t NN)
{
    if (is_debug) std::cout << "3XNN: SKIP IF V[" << (int)X << "] == " << (int)NN << std::endl;
    if (V[X] == NN)
    {
        program_counter += 2;
    }
}

void Chip8::OP_4XNN(std::uint8_t X, std::uint8_t NN)
{
    if (is_debug) std::cout << "4XNN: SKIP IF V[" << (int)X << "] != " << (int)NN << std::endl;
    if (V[X] != NN)
    {
        program_counter += 2;
    }
}

void Chip8::OP_5XY0(std::uint8_t X, std::uint8_t Y)
{
    if (is_debug) std::cout << "5XY0: SKIP IF V[" << (int)X << "] == V[" << (int)Y << "]" << std::endl;
    if (V[X] == V[Y])
    {
        program_counter += 2;
    }
}

void Chip8::OP_6XNN(std::uint8_t X, std::uint8_t NN)
{
    if (is_debug) std::cout << "6XNN: SET V[" << (int)X << "] TO " << (int)NN << std::endl;
    V[X] = NN;
}

void Chip8::OP_7XNN(std::uint8_t X, std::uint8_t NN)
{
    if (is_debug) std::cout << "7XNN: ADD " << (int)NN << " TO V[" << (int)X << "]" << std::endl;
    V[X] += NN; 
}

void Chip8::OP_8XYN(std::uint8_t X, std::uint8_t Y, std::uint8_t N)
{
    switch (N)
    {
        case 0x0:
            if (is_debug) std::cout << "8XY0: V[" << (int)X << "] = V[" << (int)Y << "]" << std::endl;
            V[X] = V[Y];
            break;
        case 0x1:
            if (is_debug) std::cout << "8XY1: V[" << (int)X << "] |= V[" << (int)Y << "]" << std::endl;
            V[X] = V[X] | V[Y];
            break;
        case 0x2:
            if (is_debug) std::cout << "8XY2: V[" << (int)X << "] &= V[" << (int)Y << "]" << std::endl;
            V[X] = V[X] & V[Y];
            break;
        case 0x3:
            if (is_debug) std::cout << "8XY3: V[" << (int)X << "] ^= V[" << (int)Y << "]" << std::endl;
            V[X] = V[X] ^ V[Y];
            break;
        case 0x4:
            if (is_debug) std::cout << "8XY4: V[" << (int)X << "] += V[" << (int)Y << "] WITH CARRY" << std::endl;
            if (V[X] + V[Y] > 255)
            {
                V[X] = V[X] + V[Y];      
                V[REGISTER_SIZE - 1] = 1;
            }
            else 
            {
                V[X] = V[X] + V[Y];    
                V[REGISTER_SIZE - 1] = 0;     
            }
            break;
        case 0x5:
            if (is_debug) std::cout << "8XY5: V[" << (int)X << "] -= V[" << (int)Y << "] WITH BORROW" << std::endl;
            if (V[X] > V[Y])
            {
                V[REGISTER_SIZE - 1] = 1;
            }
            else
            {
                V[REGISTER_SIZE - 1] = 0;
            } 
            V[X] = V[X] - V[Y];
            break;
        case 0x6:
            if (is_debug) std::cout << "8XY6: SHIFT RIGHT V[" << (int)X << "]" << std::endl;
            V[X] = V[Y];
            V[REGISTER_SIZE - 1] = V[X] & 0x1;
            V[X] = V[X] >> 1;
            break;
        case 0x7:
            if (is_debug) std::cout << "8XY7: V[" << (int)X << "] = V[" << (int)Y << "] - V[" << (int)X << "]" << std::endl;
            if (V[Y] > V[X])
            {
                V[REGISTER_SIZE - 1] = 1;
            }
            else
            {
                V[REGISTER_SIZE - 1] = 0;
            } 
            V[X] = V[Y] - V[X];
            break;
        case 0xE:
            if (is_debug) std::cout << "8XYE: SHIFT LEFT V[" << (int)X << "]" << std::endl;
            V[X] = V[Y];
            V[REGISTER_SIZE - 1] = (V[X] & 0x80) >> 7;
            V[X] = V[X] << 1;
            break;
    }
}

void Chip8::OP_9XY0(std::uint8_t X, std::uint8_t Y)
{
    if (is_debug) std::cout << "9XY0: SKIP IF V[" << (int)X << "] != V[" << (int)Y << "]" << std::endl;
    if (V[X] != V[Y])
    {
        program_counter += 2;
    }
}

void Chip8::OP_ANNN(std::uint16_t NNN)
{
    if (is_debug) std::cout << "ANNN: SET INDEX REGISTER TO " << std::hex << NNN << std::dec << std::endl;
    I = NNN;
}

void Chip8::OP_BNNN(std::uint16_t NNN)
{
    if (is_debug) std::cout << "BNNN: JUMP TO " << std::hex << NNN << " + V[0]" << std::dec << std::endl;
    program_counter = NNN + V[0];
}

void Chip8::OP_CXNN(std::uint8_t X, std::uint8_t NN)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 255);
    std::uint8_t random_num = distr(gen);
    V[X] = random_num & NN;
}

 void Chip8::OP_DXYN(std::uint8_t X, std::uint8_t Y, std::uint8_t N)
 {
    if (is_debug) std::cout << "DXYN: DRAW SPRITE AT V[" << (int)X << "], V[" << (int)Y << "]" << std::endl;
    std::uint8_t X_coord = V[X] % DISPLAY_WIDTH;
    std::uint8_t Y_coord = V[Y] % DISPLAY_HEIGHT;
    std::uint8_t mask = 1; 
    V[REGISTER_SIZE - 1] = 0;
    for (int i = 0; i < N; i++){
        std::uint8_t sprite_row = memory[I + i];
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
 }