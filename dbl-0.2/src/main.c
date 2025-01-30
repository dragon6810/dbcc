#include <stdio.h>
#include <stdlib.h>

#include <cli/cli.h>
#include <list/list.h>

int main(int argc, char** argv)
{
    int i;

    list_t sourcefiles;

    if(argc == 1)
    {
        cli_printusage();
        abort();
    }

    list_initialize(&sourcefiles, sizeof(char*));
    for(i=1; i<argc; i++)
    {
        if(argv[i][0] != '-')
        {
            list_push(&sourcefiles, &argv[i]);
            continue;
        }
    }

    printf("source files:\n");
    for(i=0; i<sourcefiles.size; i++)
        puts(((char**)sourcefiles.data)[i]);

    return 0;
}