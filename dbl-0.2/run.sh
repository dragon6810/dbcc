#!/usr/bin/env bash

RUN_DIR="run"

mkdir -p "$RUN_Dir"

if make; then
    cd "$RUN_DIR"
    ../bin/dbc -v main.c

    exit 0
fi

exit 1