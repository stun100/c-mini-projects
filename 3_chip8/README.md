# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using [raylib](https://www.raylib.com/) for rendering.

CHIP-8 is an interpreted programming language developed in the mid-1970s, designed to run on simple virtual machines. It was originally used to write games for 8-bit microcomputers.

## Features

- Full CHIP-8 instruction set implementation
- 64×32 pixel display rendered with raylib
- Debug mode: step through instructions one at a time and inspect registers

## Requirements

- A C++20 compiler (e.g. `clang++` or `g++`)
- [CMake](https://cmake.org/) ≥ 3.10
- [raylib](https://www.raylib.com/)

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```bash
./chip8 [options] [rom_file]
```

| Argument | Description |
|---|---|
| *(none)* | Loads the default ROM (`../games/1-chip8-logo.ch8`) |
| `[rom_file]` | Path to a `.ch8` ROM file to load |
| `-d [rom_file]` | Enable debug mode (optional ROM path) |

### Debug Mode

In debug mode the window shows a register sidebar (V0–VF) and the current opcode. Press **Space** to advance one CPU cycle at a time.

## Games / ROMs

Several test ROMs are included in the `games/` directory:

| File | Description |
|---|---|
| `1-chip8-logo.ch8` | CHIP-8 logo |
| `2-ibm-logo.ch8` | IBM logo |
| `3-corax+.ch8` | Opcode test suite |
| `4-flags.ch8` | Flags / VF register tests |
| `test_opcode.ch8` | General opcode tests |
| `Connect 4 [David Winter].ch8` | Connect 4 game |

## Todo

- [ ] Implement keyboard input (opcodes `EX9E` / `EXA1` / `FX0A`)
- [ ] Implement timer opcodes (`FX07`, `FX15`, `FX18`)
- [ ] Implement remaining `F` opcodes (`FX29`, `FX33`, `FX55`, `FX65`)
- [ ] Add sound support
