#include "error.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void Error(char* msg, ...)
{
    va_list args;

    printf("ERROR: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    exit(1);
}
