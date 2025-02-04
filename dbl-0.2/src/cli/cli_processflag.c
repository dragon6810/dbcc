#include <cli/cli.h>

#include <string.h>

void cli_processflag_processincludedir(const char* flag)
{
    const char *start;

    start = flag + 2;
    list_push(&cli_includedirs, &start);
}

void cli_processflag(const char* flag)
{
    if(flag[0] != '-' || strlen(flag) < 2)
        return;

    switch(flag[1])
    {
    case 'v':
    case 'V':
        cli_verbose = true;
        break;
    case 'I':
        cli_processflag_processincludedir(flag);
        break;
    }
}