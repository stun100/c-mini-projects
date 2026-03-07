#include "raylib.h"
#include "chip8.h"
#include <iostream>
#include <filesystem>
#include <cstring>

const int SCALE = 18;

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
        InitWindow(chip8.DISPLAY_WIDTH*SCALE + 200, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8 - Debug");
    }
    else 
    {
        InitWindow(chip8.DISPLAY_WIDTH*SCALE, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8");
    }
   
    SetTargetFPS(120);
    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);
        
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
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, WHITE);
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