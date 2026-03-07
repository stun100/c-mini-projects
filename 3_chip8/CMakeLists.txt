cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} 
    main.cpp 
    Chip8.cpp
    Chip8.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} raylib)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        "-framework IOKit" 
        "-framework Cocoa" 
        "-framework OpenGL" 
        "-framework CoreVideo"
    )
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        winmm
        gdi32
        opengl32
    )
endif()