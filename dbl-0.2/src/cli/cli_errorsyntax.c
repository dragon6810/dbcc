#include <cli/cli.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>

void cli_errorsyntax(const char* file, unsigned long int line, unsigned long int column, const char* format, ...)
{
    va_list args;
    
    va_start(args, format);
    cli_verrorsyntax(file, line, column, format, args);
    va_end(args);
}