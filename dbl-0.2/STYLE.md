# Deadbird Compiler Style Guide

## Source Hierarchy & Header Files
When creating a new functionality (1 header file), create a subdirectory with that header file's name. The example I'll be using going forward is `list/list.h`. Include gaurds should be `_[header name]_h`, for example `_list_h`. In that subdirectory, every external function should have it's own c file. Any `extern` variables that need defenitions should be put in a c file with `[header name]_defs.c`. For example, `list/list.h` would (if it had any extern variables) would have a source file named `list/list_defs.c`. All function names should be named with the header file's namespace. For example `list_push`. Within a c file, private functions should be declared static and the namespace includes the c file. For example, in `list/list_shuffle.c` there is `static void list_shuffle_swap`.

## Indentation
All indentation should be done with 4 spaces.

## Braces / Parentheses
We use new-line curly braces, always. Also, if you are expanding any function call or declaration/definition, the parentheses should be on a newline. For example:

    somefunc
    (
        parama,
        paramb
    );

This also applys for square brackets if you need to index an array on a new line for some reason

    somearray
    [
        y*w+x
    ];

## Variable Declaration on the Stack
When writing a function, you must declare every variable you will use in the function before any logic. For example:

    somefunc(int para, int parb)
    {
        int var1, var2;
        float var3;
        char var4[16];

        ... (Logic) ...
    }

