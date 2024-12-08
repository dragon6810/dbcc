#!/bin/bash

if make; then
    bin/dbl -c ../hello/src/main.c
else
    exit 1
fi

exit 0
