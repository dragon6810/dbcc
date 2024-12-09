#include "lexer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
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
    while (either) 
    {
        either = false;
        if (txt_skipwhitespace(ptr))
            either = true;

        if ((c = fgetc(ptr)) == '#') 
        {
            txt_skipline(ptr);
            either = true;
        } 
        else if (c != EOF) 
        {
            fseek(ptr, -1, SEEK_CUR);
        }

        if (txt_skipwhitespace(ptr))
            either = true;
    }

    start = ftell(ptr);
    fseek(ptr, 0, SEEK_END);
    if (ftell(ptr) == start) 
    {
        str = calloc(1, 1);
        return str;
    }

    fseek(ptr, start, SEEK_SET);
    while ((c = fgetc(ptr)) != EOF && c > 32);
    if (c != EOF)
        fseek(ptr, -1, SEEK_SET);
    end = ftell(ptr) - 1;

    str = malloc(end - start + 2);
    if (!str) 
    {
        // Handle memory allocation error
        return NULL;
    }

    fseek(ptr, start, SEEK_SET);
    fread(str, 1, end - start, ptr);
    str[end - start] = '\0';

    return str;
}

bool tkn_loadfile(FILE* ptr) 
{
    int c;
    char* str;
    tokenfile_t newfile;
    token_t newtoken;
    unsigned long int linenum, charnum, realnum;
    bool ignorerestofline;
    bool insidequotes;

    linenum = 1;
    charnum = 1;
    realnum = 0;
    ignorerestofline = false;
    insidequotes = false;

    newfile.filename = "test";
    darr_init(&newfile.tokens, sizeof(token_t));

    // Initialize `str` as an empty string
    str = calloc(1, 1);
    if (!str) 
    {
        // Handle allocation error
        return false;
    }

    while ((c = fgetc(ptr)) != EOF) 
    {
        realnum++;

        if (c == '\n') 
        {
            linenum++;
            charnum = 1;
            ignorerestofline = false;
            continue;
        } 
        else 
        {
            charnum++;
        }

        if (ignorerestofline) 
        {
            continue;
        }

        if (c == '"') 
        {
            insidequotes = !insidequotes;
        }

        // If a space or a semicolon is encountered, create a new token from str
        if ((isspace(c) || c == ';' ) && !insidequotes) 
        {
            if (strlen(str) > 0) 
            {
                // Create a new token from str
                newtoken.linenum = linenum;
                newtoken.charnum = charnum;
                newtoken.realnum = realnum;
                newtoken.payload = str;  // Pass ownership of str
                newtoken.node = NULL;

                // Add the token to the file
                darr_push(&newfile.tokens, &newtoken);

                // Reset `str` to a new empty string
                str = calloc(1, 1);
                if (!str) 
                {
                    // Handle allocation error
                    return false;
                }
            }
        } 
        else 
        {
            // Add the character to `str`
            size_t len = strlen(str);
            char* temp = realloc(str, len + 2); // Allocate space for new char and null terminator
            if (!temp) 
            {
                // Handle reallocation error
                free(str);
                return false;
            }
            str = temp;
            str[len] = c;
            str[len + 1] = '\0'; // Null terminate the string
        }
    }

    // Handle any remaining token at the end of the file
    if (strlen(str) > 0) 
    {
        newtoken.linenum = linenum;
        newtoken.charnum = charnum;
        newtoken.realnum = realnum;
        newtoken.payload = str;  // Pass ownership of str
        newtoken.node = NULL;

        darr_push(&newfile.tokens, &newtoken);
    } 
    else 
    {
        free(str);
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
    for (i = 0; i < tokenfiles.len; i++) 
    {
        file = &filesdata[i];
        tokensdata = (token_t*)file->tokens.data;
        printf("File: %s\n", file->filename);
        for (j = 0; j < file->tokens.len; j++) 
        {
            token = &tokensdata[j];
            printf("Token: %s Line: %d Column: %d\n", token->payload, token->linenum, token->charnum);
        }
    }

    return true;
}
