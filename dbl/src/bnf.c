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

    if(c != EOF)
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
        
        if((c = fgetc(ptr)) == '#')
        {
            bnf_skipline(ptr);
            either = true;
        }
        else if(c != EOF)
            fseek(ptr, -1, SEEK_CUR);
    
        if(bnf_skipwhitespace(ptr))
            either = true;
    }
    
    start = ftell(ptr);    
    fseek(ptr, 0, SEEK_END);
    if(ftell(ptr) == start)
    {
        str = calloc(1, 1);
        return str;
    }

    fseek(ptr, start, SEEK_SET);
    while((c = fgetc(ptr)) != EOF && c > 32);
    if(c != EOF)
        fseek(ptr, -1, SEEK_SET);
    else
        printf("eof reached\n");
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
    darr_t tokens; // Array of char*

    ptr = fopen(filepath, "r");
    if(!ptr)
    {
        Error("couldn't open bnf specification file \"%s\".\n", filepath);
        return false;
    }

    str = 0;
    while(!str || str[0])
    {
        if(str)
            free(str);

        str = bnf_nexttoken(ptr);
        if(str[0])
            puts(str);
    }
    (void) fclose(ptr);
    return true;
}
