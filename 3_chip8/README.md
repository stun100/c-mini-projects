# CHIP-8 Emulator

This is a CHIP-8 emulator written in C++ using [raylib](https://www.raylib.com/) for rendering.

To build and run the emulator, use the following commands:

```bash
mkdir build && cd build
cmake ..
cmake --build .
./chip8 [rom_file]
```

Pass `-d` before the ROM path to enable debug mode, where you can step through instructions one cycle at a time by pressing **Space**.

Several test ROMs are included in the `games/` directory.

## Todo

- [ ] Implement keyboard input
- [ ] Implement timers and sound
- [ ] Implement remaining opcodes
