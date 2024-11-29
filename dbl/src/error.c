#include "error.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void Error(char* msg, ...)
{
    va_list args;

    (void) printf("ERROR: ");

    (void) va_start(args, msg);
    (void) vprintf(msg, args);
    (void) va_end(args);
    (void) exit(1);
}
