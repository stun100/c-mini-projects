cmake_minimum_required(VERSION 3.10)
project(chip8)

# --- THE FIX: Change 17 to 20 ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Prevents the compiler from using "gnu++20" (keeps it standard C++20)
set(CMAKE_CXX_EXTENSIONS OFF) 

# Find raylib
find_package(raylib REQUIRED)

# Add sources
add_executable(${PROJECT_NAME} 
    main.cpp 
    Chip8.cpp
    Chip8.h
)

# Include current directory
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link raylib
target_link_libraries(${PROJECT_NAME} raylib)

# macOS specific framework linking
if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        "-framework IOKit" 
        "-framework Cocoa" 
        "-framework OpenGL" 
        "-framework CoreVideo"
    )
endif()