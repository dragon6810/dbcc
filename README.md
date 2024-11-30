# dbcc

(D)ead(b)ird (C)ompiler (C)ollection

## Development

### To build dbl

To build dbl, `cd` into the `dbl/` directory
and run `sh run.sh`. It will generate and
run an executable file named `dbl` in the the `bin` directory.

### hello

The repo contains a small test program for compiling in the `hello/` directory.

#### hello with clang

To compile it with clang:

    cd hello/
    clang src/main.c -o bin/main

To see intermediate steps from clang:

    clang -fsyntax-only -Xclang -dump-tokens src/main.c
    clang -fsyntax-only -Xclang -ast-dump src/main.c
    clang -S -emit-llvm src/main.c -o bin/main.ll
