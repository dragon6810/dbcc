#!/usr/bin/env bash

make clean
if bear -- make; then
    exit 0
fi

exit 1
