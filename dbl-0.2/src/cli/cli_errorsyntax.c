#include <cli/cli.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert/assert.h>

void cli_errorsyntax_printline(const char* file, unsigned long int line, unsigned long int column)
{
    unsigned long int i, j;

    FILE *ptr;
    unsigned long int filelen;
    char *data, *str;

    ptr = fopen(file, "r");
    if(!ptr)
        return;

    fseek(ptr, 0, SEEK_END);
    filelen = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    data = malloc(filelen + 1);
    fread(data, 1, filelen, ptr);
    data[filelen] = 0;

    for(i=j=0; i<filelen && j<line; i++)
    {
        if(data[i] == '\n')
            j++;
    }

    for(j=i; j<filelen; j++)
    {
        if(data[j] == '\n')
            break;
    }

    str = malloc(j + 1);
    memcpy(str, data + i, j - i);
    str[j - i] = 0;

    printf("%5lu | %s\n", line + 1, str);

    printf("      | ");
    for(j=0; j<column; j++)
    {
        assert(str[j]);
        if(str[j] <= 32)
        {
            printf("%c", str[j]);
            continue;
        }

        printf(" ");
    }
    
    printf("\033[32m^\033[0m\n");

    free(str);
    free(data);
    fclose(ptr);
}

void cli_errorsyntax(const char* file, unsigned long int line, unsigned long int column, const char* format, ...)
{
    va_list args;
    
    va_start(args, format);

    printf("\033[0;1m%s:%lu:%lu: ", file, line + 1, column + 1);
    printf("\033[31;1merror: ");

    printf("\033[0;1m");
    printf(format, args);
    printf("\n");

    printf("\033[0m");
    cli_errorsyntax_printline(file, line, column);

    va_end(args);
}