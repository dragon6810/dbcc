#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "error.h"
#include "lexer.h"
#include "lexer_bnf.h"
#include "bnf.h"

bool linking = true;

void opensource(char* filename)
{
    FILE* ptr;
    
    ptr = fopen(filename, "r");
    // tkn_loadfile(ptr);
    fclose(ptr);

    tkn_bnf_loadfile(filename);
}

int main(int argc, char** argv)
{
    int i;

    bool insources;
    FILE* ptr;

    bnf_loadspec("simple.bnf"); 
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

        opensource(argv[i]);
        fclose(ptr);
    }

    //print_tokens();

    return 0;
}
