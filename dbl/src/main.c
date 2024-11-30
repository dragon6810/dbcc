#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "error.h"
#include "token.h"
#include "bnf.h"

bool linking = true;

void opensource(FILE* ptr)
{
    int i;

    unsigned long int fsize;
    char* buff;
    darr_t arr;

    fseek(ptr, 0, SEEK_END);
    fsize = ftell(ptr);

    buff = malloc(fsize + 1);

    buff[fsize] = 0;
    (void) fseek(ptr, 0, SEEK_SET);
    (void) fread(buff, 1, fsize, ptr);

    arr = tokenize(buff);
    (void) bnf_loadspec("c.bnf"); 
    for(i=0; i<arr.len; i++)
        (void) free(((token_t*) arr.data)[i].payload);

    (void) free(arr.data);
    (void) free(buff);
}

int main(int argc, char** argv)
{
    int i;

    bool insources;
    FILE* ptr;

    for(i=1, insources=false; i<argc; i++)
    {
        if(argv[i][0] != '-')
            insources = true;

        if(!strcmp(argv[i], "-c"))
        {
            if(insources)
                (void) Error("giving argument -c is illegal after sources have begun.\n");
            
            linking = false;
            continue;
        }

        if(!insources)
            (void) Error("unexpected arg \"%s\".\n", argv[i]);
    
        ptr = fopen(argv[i], "r");
        if(!ptr)
            (void) Error("can't open file \"%s\".\n", argv[i]);

        (void) opensource(ptr);
        (void) fclose(ptr);
    }

    return 0;
}
