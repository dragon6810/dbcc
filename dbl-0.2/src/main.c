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
            printf("    %s\n", sourcefilenames.data[i]);
    }

    LIST_RESIZE(cli_allunits, sourcefilenames.size);
    for(i=0; i<cli_allunits.size; i++)
    {
        if(!srcfile_load(sourcefilenames.data[i], &cli_allunits.data[i]))
        {
            cli_errornofile("source", sourcefilenames.data[i]);
            abort();
        }

        srcfile_compile(&cli_allunits.data[i]);
    }

    for(i=0; i<cli_allunits.size; i++)
        srcfile_free(&cli_allunits.data[i]);
    
    LIST_FREE(cli_allunits);
    LIST_FREE(sourcefilenames);
    
    return 0;
}
