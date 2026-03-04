#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <stack>
#include <string>
#include <cstdint>
#include <vector>

class Chip8 {

    public:
        static constexpr std::uint16_t MEMORY_SIZE = 4096;
        static constexpr std::uint8_t REGISTER_SIZE = 16;
        static constexpr std::uint8_t DISPLAY_WIDTH = 64;
        static constexpr std::uint8_t DISPLAY_HEIGHT = 32;
        static constexpr std::uint8_t FONTSET_SIZE = 80;
        static constexpr std::uint8_t FONTSET_START_ADDRESS = 0x50;
        static constexpr std::uint16_t ROM_START_ADDRESS = 0x200;

        Chip8();

        bool load_rom(const std::string& file_path);
        void cycle();
        

    private:
        std::array<uint8_t, MEMORY_SIZE> memory{};
        std::array<uint8_t, REGISTER_SIZE> V{};
        std::array<std::array<uint8_t, DISPLAY_HEIGHT>, DISPLAY_WIDTH> display{{}};
        std::stack<uint16_t> stack;

        uint16_t I{0};
        uint16_t program_counter{ROM_START_ADDRESS};
        uint8_t delay_timer{0};
        uint8_t sound_timer{0};

        void load_fontset();
        std::uint16_t fetch();
        void decode(std::uint16_t instruction);
        void execute();

        static const std::uint8_t fontset[FONTSET_SIZE];
};

#endif