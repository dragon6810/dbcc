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

bnf_spec_tree_t bnf_nodepass(darr_t tokens)
{
    int i;

    bnf_spec_tree_t tree;

    char** tokensdata = (char**) tokens.data;

    darr_init(&tree.nodes, sizeof(bnf_spec_node_t));
    for(i=0; i<tokens.len; i++)
    {
        printf("Token: \"%s\".\n", tokensdata[i]);     
    }

    return tree; 
}

bool bnf_loadspec(char* filepath)
{
    int i;

    FILE* ptr;
    char* str;
    darr_t tokens; // Array of char*
    char** tokensdata;

    ptr = fopen(filepath, "r");
    if(!ptr)
    {
        Error("couldn't open bnf specification file \"%s\".\n", filepath);
        return false;
    }

    darr_init(&tokens, sizeof(char*));
    while(true)
    {
        str = bnf_nexttoken(ptr);
        if(!str[0])
            break;

        darr_push(&tokens, &str);
    }
    free(str);

    (void) fclose(ptr);
   
    bnf_nodepass(tokens);

    tokensdata = (char**) tokens.data;
    for(i=0; i<tokens.len; i++)
        free(tokensdata[i]);

    free(tokensdata); 
        
    return true;
}
