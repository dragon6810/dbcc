#include <cli/cli.h>

#include <stdio.h>

#include <std/assert/assert.h>

void cli_errornofile(const char* reason, const char* path)
{
    assert(reason);
    assert(path);

    printf("\x1B[31merror: \x1B[0m");
    printf("can't open %s file \x1B[33m%s\x1B[0m\n", reason, path);
}