#include "bnf.h"

#include <stdio.h>

#include "error.h"

bnf_spec_tree_t spec;

void bnf_skipline(FILE* ptr)
{
    char c;

    while((c = fgetc(ptr)) != '\n' && c != EOF);
}

bool bnf_skipwhitespace(FILE* ptr)
{
    char c;
    bool did;

    did = false;
    while((c = fgetc(ptr)) <= 32 && c != EOF)
        did = true;

    fseek(ptr, -1, SEEK_CUR);
    return did;
}

char* bnf_nexttoken(FILE* ptr)
{
    char c;
    bool either;

    char* str;

    unsigned long int start;
    unsigned long int end;

    either = true;
    while(either)
    {
        either = false;
        
        if(bnf_skipwhitespace(ptr))
            either = true;
        
        if(fgetc(ptr) == '#')
        {
            bnf_skipline(ptr);
            either = true;
        }
        else
            fseek(ptr, -1, SEEK_CUR);
    }
    
    start = ftell(ptr);    
    while((c = fgetc(ptr)) > 32 && c != EOF);
    end = ftell(ptr);

    str = malloc(end - start + 1);
    fseek(ptr, start, SEEK_SET);
    fread(str, 1, end - start, ptr);
    str[end - start] = 0;
    
    return str;
}

bool bnf_loadspec(char* filepath)
{
    FILE* ptr;
    char* str;

    ptr = fopen(filepath, "r");
    if(!ptr)
    {
        Error("couldn't open bnf specification file \"%s\".\n", filepath);
        return false;
    }

    str = bnf_nexttoken(ptr);
    puts(str);
    free(str);

    (void) fclose(ptr);
    return true;
}
