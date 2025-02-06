#include <stdio.h>
#include <stdlib.h>

#include <cli/cli.h>
#include <hashmap/hashmap.h>
#include <hashmap/int32/int32.h>
#include <list/list.h>
#include <srcfile/srcfile.h>

void testhashmap(void)
{
    hashmap_t map;
    int key, val;

    hashmap_initialize(&map, hashmap_int32_hash, hashmap_int32_cmp, HASHMAP_BUCKETS_MEDIUM, sizeof(int), sizeof(int), NULL, NULL);

    key = 42;
    val = 64;
    hashmap_set(&map, &key, &val);
    val = *((int*)hashmap_fetch(&map, &key));
    printf("val: %d.\n", val);

    hashmap_free(&map);
}

int main(int argc, char** argv)
{
    int i;

    list_t sourcefilenames;
    list_t sourcefiles;

    cli_initialize();

    if(argc == 1)
    {
        cli_printusage();
        abort();
    }

    testhashmap();
    return 0;
    
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
            printf("    %s\n", LIST_FETCH(sourcefilenames, char*, i));
    }

    list_initialize(&sourcefiles, sizeof(srcfile_t));
    list_resize(&sourcefiles, sourcefilenames.size);
    for(i=0; i<sourcefiles.size; i++)
    {
        if(!srcfile_load(LIST_FETCH(sourcefilenames, char*, i), &LIST_FETCH(sourcefiles, srcfile_t, i)))
        {
            cli_errornofile("source", LIST_FETCH(sourcefilenames, char*, i));
            abort();
        }

        lexer_tknfile(&LIST_FETCH(sourcefiles, srcfile_t, i));
    }

    for(i=0; i<sourcefiles.size; i++)
        srcfile_free(&LIST_FETCH(sourcefiles, srcfile_t, i));
    
    list_free(&sourcefiles);
    list_free(&sourcefilenames);
    
    return 0;
}
