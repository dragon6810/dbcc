#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "error.h"
#include "lexer.h"
#include "bnf.h"

bool linking = true;

void opensource(FILE* ptr)
{
    tkn_loadfile(ptr);
}

int main(int argc, char** argv)
{
    int i;

    bool insources;
    FILE* ptr;

    bnf_loadspec("c.bnf"); 
    tkn_init();

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
