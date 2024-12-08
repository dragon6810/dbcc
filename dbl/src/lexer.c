#include "lexer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "bnf.h"
#include "textutils.h"

darr_t tokenfiles;

void tkn_init(void)
{
    darr_init(&tokenfiles, sizeof(tokenfile_t));
}

char* nexttoken(FILE* ptr)
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
        
        if(txt_skipwhitespace(ptr))
            either = true;
        
        if((c = fgetc(ptr)) == '#')
        {
            txt_skipline(ptr);
            either = true;
        }
        else if(c != EOF)
            fseek(ptr, -1, SEEK_CUR);
    
        if(txt_skipwhitespace(ptr))
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
    end = ftell(ptr) - 1;

    str = malloc(end - start + 1);
    fseek(ptr, start, SEEK_SET);
    fread(str, 1, end - start, ptr);
    str[end - start] = 0;
    
    return str;
}

bool tkn_loadfile(FILE* ptr)
{
    int c;
    char* str;
    char* token;
    unsigned long int linenum, charnum, realnum;
    tokenfile_t newfile;
    token_t newtoken;
    bnf_spec_node_t* nodesdata;
    bnf_spec_node_t* curnode;
    int i;
    bool either;

    linenum = 1;
    charnum = 1;
    realnum = 0;
    either = true;

    // Start ingesting the file one character at a time,
    // incrementing counters.
    // As test, store each character as a token. The 
    // bnf_spec_node_t *node can be null.

    newfile.filename = "test";
    darr_init(&newfile.tokens, sizeof(token_t));

    // Loop through the file
     

    while ((c = fgetc(ptr)) != EOF) {
        // Skip whitespace
        if (c == '\n') {
            linenum++;
            charnum = 1;
        } else {
            charnum++;
        }

        if (c == '#') {
            txt_skipline(ptr);
            continue;
        }

        if (c <= 32) {
            continue;
        }

        // Create a new token
        newtoken.linenum = linenum;
        newtoken.charnum = charnum;
        newtoken.realnum = realnum;
        newtoken.payload = calloc(1, 2);
        newtoken.payload[0] = c;
        newtoken.payload[1] = 0;
        newtoken.node = NULL;

        // Add the token to the file
        darr_push(&newfile.tokens, &newtoken);
    }
   

    darr_push(&tokenfiles, &newfile);

    return true;
}

bool print_tokens(void)
{
    int i, j;

    tokenfile_t* filesdata;
    token_t* tokensdata;

    tokenfile_t* file;
    token_t* token;

    filesdata = (tokenfile_t*)tokenfiles.data;
    for (i = 0; i < tokenfiles.len; i++) {
        file = &filesdata[i];
        tokensdata = (token_t*)file->tokens.data;
        printf("File: %s\n", file->filename);
        for (j = 0; j < file->tokens.len; j++) {
            token = &tokensdata[j];
            printf("Token: %s Line: %d Column: %d\n", token->payload, token->linenum, token->charnum);
        }
    }

    return true;
}