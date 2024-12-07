#!/bin/bash

if make; then
    bin/dbl -c samples/main.dbl
else
    exit 1
fi

exit 0
