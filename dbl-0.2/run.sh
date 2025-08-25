#!/usr/bin/env bash

RUN_DIR="run"

mkdir -p "$RUN_Dir"

if make; then
    cd "$RUN_DIR"
    ../bin/dbc -v -I. simple.c
    as -arch arm64 -o out.o out.s
    ld -o outbin out.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64 
    chmod +x out

    exit 0
fi

exit 1