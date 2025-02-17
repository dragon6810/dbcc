#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <cli/cli.h>
#include <srcfile/srcfile.h>
#include <std/hashmap/hashmap.h>
#include <std/hashmap/int32/int32.h>
#include <std/hashmap/string/string.h>
#include <std/list/list.h>
#include <std/profiler/profiler.h>

int main(int argc, char** argv)
{
    int i;

    list_str_t sourcefilenames;

    profiler_setup();

    profiler_push("Compile");

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
        cli_printwelcome();

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

    profiler_pop();
    
    return 0;
}
