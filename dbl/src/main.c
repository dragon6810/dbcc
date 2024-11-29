#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "error.h"
#include "token.h"

bool linking = true;

void opensource(FILE* ptr)
{
    unsigned long int fsize;
    char* buff;

    fseek(ptr, 0, SEEK_END);
    fsize = ftell(ptr);

    buff = malloc(fsize + 1);

    buff[fsize] = 0;
    fseek(ptr, 0, SEEK_SET);
    fread(buff, 1, fsize, ptr);

    tokenize(buff);

    free(buff);
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
                Error("giving argument -c is illegal after sources have begun.\n");
            
            linking = false;
            continue;
        }

        if(!insources)
            Error("unexpected arg \"%s\".\n", argv[i]);
    
        ptr = fopen(argv[i], "r");
        if(!ptr)
            Error("can't open file \"%s\".\n", argv[i]);

        opensource(ptr);
        fclose(ptr);
    }

    return 0;
}
