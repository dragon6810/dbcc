#include "token.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

darr_t tokenize(char* buff)
{
    char* c;
    char* start;
    
    bool inquote;
    char* curstr;
    token_t curtoken;

    darr_t arr;

    darr_init(&arr, sizeof(token_t));
    for(c = start = buff, inquote = false; c<buff+strlen(buff); c++)
    {
        if(!(c - start))
            continue;

        if(*c == '"' && !inquote)
        {
            inquote = true;
            start = c + 1;
            continue;
        }

        if(*c == '"' && inquote)
        {
            inquote = false;
            goto valid;
        }
            
        if(*c <= 32 && !inquote)
            goto valid;

        if(*c == ';')
            goto valid;

        if(*c == '"')
            goto valid;

        continue;
valid:
        while(*start <= 32 && ((start - buff) < strlen(buff)))
            start++;

        if(start >= c)
            continue;

        curstr = malloc(c - start + 1);
        memcpy(curstr, start, c - start);
        curstr[c - start] = 0;

        curtoken.payload = curstr;
        switch(*c)
        {
        case '"':
            curtoken.type = TOKEN_STRING;
            break;
        default:
            curtoken.type = TOKEN_LABEL;
            break;
        }

        printf("Token: \"%s\".\n", curstr);
        darr_push(&arr, &curtoken);

        if(*c != '"')
            start = c;
        else
            start = c + 1;
    }
    
    return arr;
}
