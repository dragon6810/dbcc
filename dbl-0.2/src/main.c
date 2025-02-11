#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <cli/cli.h>
#include <hashmap/hashmap.h>
#include <hashmap/int32/int32.h>
#include <hashmap/string/string.h>
#include <list/list.h>
#include <srcfile/srcfile.h>

int main(int argc, char** argv)
{
    int i;

    list_str_t sourcefilenames;
    list_srcfile_t sourcefiles;
    list_lexer_state_t lexerstates;

    cli_initialize();

    if(argc == 1)
    {
        cli_printusage();
        abort();
    }
    
    LIST_INITIALIZE(sourcefilenames);
    for(i=1; i<argc; i++)
    {
        if(argv[i][0] != '-')
        {
            LIST_PUSH(sourcefilenames, argv[i]);
            continue;
        }

        cli_processflag(argv[i]);
    }

    if(cli_verbose)
    {
        cli_printwelcome();

        printf("source files:\n");
        for(i=0; i<sourcefilenames.size; i++)
            printf("    %s\n", LIST_FETCH(sourcefilenames, char*, i));
    }

    LIST_INITIALIZE(sourcefiles);
    LIST_INITIALIZE(lexerstates);
    LIST_RESIZE(sourcefiles, sourcefilenames.size);
    LIST_RESIZE(lexerstates, sourcefilenames.size);
    for(i=0; i<sourcefiles.size; i++)
    {
        if(!srcfile_load(sourcefilenames.data[i], &sourcefiles.data[i]))
        {
            cli_errornofile("source", LIST_FETCH(sourcefilenames, char*, i));
            abort();
        }

        lexer_tknfile(&LIST_FETCH(lexerstates, lexer_state_t, i), &LIST_FETCH(sourcefiles, srcfile_t, i));
    }

    for(i=0; i<sourcefiles.size; i++)
        srcfile_free(&sourcefiles.data[i]);
    
    LIST_FREE(sourcefiles);
    LIST_FREE(sourcefilenames);
    
    return 0;
}
