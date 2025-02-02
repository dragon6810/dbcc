#include <stdio.h>
#include <stdlib.h>

#include <cli/cli.h>
#include <list/list.h>
#include <srcfile/srcfile.h>

int main(int argc, char** argv)
{
    int i;

    list_t sourcefilenames;
    list_t sourcefiles;

    if(argc == 1)
    {
        cli_printusage();
        abort();
    }

    list_initialize(&sourcefilenames, sizeof(char*));
    for(i=1; i<argc; i++)
    {
        if(argv[i][0] != '-')
        {
            list_push(&sourcefilenames, &argv[i]);
            continue;
        }

        cli_processflag(argv[i]);
    }

    if(cli_verbose)
    {
        cli_printwelcome();

        printf("source files:\n");
        for(i=0; i<sourcefilenames.size; i++)
            printf("    %s\n", ((char**)sourcefilenames.data)[i]);
    }

    list_initialize(&sourcefiles, sizeof(srcfile_t));
    list_resize(&sourcefiles, sourcefilenames.size);
    for(i=0; i<sourcefiles.size; i++)
    {
        if(!srcfile_load(((char**)sourcefilenames.data)[i], &((srcfile_t*)(sourcefiles.data))[i]))
        {
            cli_errornofile("source", ((char**)sourcefilenames.data)[i]);
            abort();
        }

        lexer_tknfile(&((srcfile_t*)(sourcefiles.data))[i]);
    }

    for(i=0; i<sourcefiles.size; i++)
        srcfile_free(&((srcfile_t*)(sourcefiles.data))[i]);
    
    list_free(&sourcefiles);
    list_free(&sourcefilenames);
    
    return 0;
}