#ifndef _token_h
#define _token_h

#include "darr.h"

typedef enum
{
    TOKEN_LABEL,
    TOKEN_STRING,
} tokentype_e;

typedef struct token_s
{
    tokentype_e type;
    char* payload;
} token_t;

darr_t tokenize(char* buff);

#endif
