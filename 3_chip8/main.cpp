#include "raylib.h"
#include "chip8.h"
#include <iostream>
#include <filesystem>

const int SCALE = 10;

int main(int argc, char* argv[])  {

    Chip8 chip8;
    chip8.load_rom("../games/ibm_logo.ch8");

    InitWindow(chip8.DISPLAY_WIDTH*SCALE, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8 - Manual Step Mode");
    // InitWindow(chip8.DISPLAY_WIDTH*SCALE + 200, chip8.DISPLAY_HEIGHT*SCALE, "CHIP-8 - Manual Step Mode");
    SetTargetFPS(60); 

    while (!WindowShouldClose()) {

        // if (IsKeyPressed(KEY_SPACE)) {
            
        // }
        chip8.cycle();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) 
            {
                if (chip8.display[x][y] != 0) 
                {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, WHITE);
                }
                
                // DrawRectangleLines(x * SCALE, y * SCALE, SCALE, SCALE, (Color){ 155, 30, 30, 255 });
            }
        }

        const auto& V = chip8.get_V();
        for (int i = 0; i < chip8.REGISTER_SIZE; i++) {
            // Formatting: V[0]: 0x00
            DrawText(TextFormat("V[%X]: 0x%02X", i, V[i]), 
                    chip8.DISPLAY_WIDTH * SCALE + 10, 10 + (i * 15), 12, RAYWHITE);
        }

        // int sidebarX = chip8.DISPLAY_WIDTH * SCALE + 10;
        // int offset = 16 * 15 + 20;

        // DrawText(TextFormat("OP: 0x%04X", chip8.get_opcode()), sidebarX, offset, 15, YELLOW);
        // DrawText(TextFormat("I : 0x%03X", chip8.get_I()), sidebarX, offset + 20, 15, RAYWHITE);

        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}