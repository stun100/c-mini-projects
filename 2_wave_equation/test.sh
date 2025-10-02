#!/bin/bash
gcc test.c -o a.out -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./a.out
