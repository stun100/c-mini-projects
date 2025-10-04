#!/bin/bash
gcc main.c -o main.out -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./main.out
