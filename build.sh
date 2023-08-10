#! /bin/bash
# This is the Linux build script
# If you are on Windows, see build.batch

# check that sdl2 is installed
if ! command -v sdl2-config &> /dev/null
then
    echo "Build failed: SDL2 isn't installed on your machine"
    exit
fi

# C Flags
CFLAGS="-Wall -g"
SDLFLAGS="$(sdl2-config --cflags) $(sdl2-config --libs)"
NAME=main

gcc $CFLAGS $SDLFLAGS -o build/$NAME *.c
