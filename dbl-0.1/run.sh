#!/bin/bash

if make; then
    bin/dbl -c samples/file.simple
else
    exit 1
fi

exit 0
