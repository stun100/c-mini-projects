#include "raylib.h"
#include "chip8.h"

int main() {
    // InitWindow(640, 320, "CHIP-8");
    // while (!WindowShouldClose()) {
    //     BeginDrawing();
    //     ClearBackground(BLACK);
    //     EndDrawing();
    // }
    // CloseWindow();

    Chip8 chip8;
    chip8.load_rom("/Users/danielebae/my_folder/projects/c-mini-projects/3_chip8/games/ibm_logo.ch8");
    chip8.fetch();

    return 0;
}