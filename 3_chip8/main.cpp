#include "raylib.h"
#include "chip8.h"
#include <iostream>
#include <filesystem>
#include <cstring>

const int SCALE = 10;

int main(int argc, char* argv[])  {
    bool debug_mode = false;
    std::string file_path = "../games/1-chip8-logo.ch8";

    if (argc >= 2) {
        if (strcmp(argv[1], "-d") == 0) {
            debug_mode = true;
            if (argc >= 3) {
                file_path = argv[2];
            }
        } else {
            file_path = argv[1];
        }
    }
    Chip8 chip8(debug_mode);
    chip8.load_rom(file_path);

    if (debug_mode)
    {
        InitWindow(chip8.DISPLAY_WIDTH*SCALE + 400, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8 - Debug");
    }
    else 
    {
        InitWindow(chip8.DISPLAY_WIDTH*SCALE, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8");
    }
   
    SetTargetFPS(700);
    while (!WindowShouldClose()) {

        chip8.current_input = 99;
        if (IsKeyDown(KEY_ONE)) chip8.current_input = 0x1;
        if (IsKeyDown(KEY_TWO)) chip8.current_input = 0x2;
        if (IsKeyDown(KEY_THREE)) chip8.current_input = 0x3;
        if (IsKeyDown(KEY_FOUR)) chip8.current_input = 0xC;
        if (IsKeyDown(KEY_Q)) chip8.current_input = 0x4;
        if (IsKeyDown(KEY_W)) chip8.current_input = 0x5;
        if (IsKeyDown(KEY_E)) chip8.current_input = 0x6;
        if (IsKeyDown(KEY_R)) chip8.current_input = 0xD;
        if (IsKeyDown(KEY_A)) chip8.current_input = 0x7;
        if (IsKeyDown(KEY_S)) chip8.current_input = 0x8;
        if (IsKeyDown(KEY_D)) chip8.current_input = 0x9;
        if (IsKeyDown(KEY_F)) chip8.current_input = 0xE;
        if (IsKeyDown(KEY_Z)) chip8.current_input = 0xA;
        if (IsKeyDown(KEY_X)) chip8.current_input = 0x0;
        if (IsKeyDown(KEY_C)) chip8.current_input = 0xB;
        if (IsKeyDown(KEY_V)) chip8.current_input = 0xF;

        BeginDrawing();
        ClearBackground(BROWN);
        
        if (debug_mode){
            if (IsKeyPressed(KEY_SPACE)) {
                chip8.cycle();
            }

            int sidebarX = chip8.DISPLAY_WIDTH * SCALE + 10;
            int offset = 16 * 15 + 20;

            DrawText(TextFormat("OP: 0x%04X", chip8.get_opcode()), sidebarX, offset, 15, YELLOW);
            DrawText(TextFormat("I : 0x%03X", chip8.get_I()), sidebarX, offset + 20, 15, RAYWHITE);
        } else 
        {
            chip8.cycle();
        }

        for (int y = 0; y < chip8.DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < chip8.DISPLAY_WIDTH; x++) 
            {
                if (chip8.display[x][y] != 0) 
                {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, GOLD);
                }
                
                if (debug_mode)
                {
                    DrawRectangleLines(x * SCALE, y * SCALE, SCALE, SCALE, (Color){ 155, 30, 30, 255 });
                }
            }
        }

        const auto& V = chip8.get_V();
        for (int i = 0; i < chip8.REGISTER_SIZE; i++) {
            // Formatting: V[0]: 0x00
            DrawText(TextFormat("V[%X]: 0x%02X", i, V[i]), 
                    chip8.DISPLAY_WIDTH * SCALE + 10, 10 + (i * 15), 12, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}