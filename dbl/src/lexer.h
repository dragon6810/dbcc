#ifndef _lexer_h
#define _lexer_h

#include <stdbool.h>

#include "darr.h"
typedef struct token_s
{
    bnf_spec_node_t *node;     // The bnf leaf this token is an instance of
    char *payload;             // The actual token
    unsigned long int linenum; // The line number this token is on
    unsigned long int charnum; // The character within the line this token is on
    unsigned long int realnum; // The character number in the file this token is on
} token_t;

typedef struct tokenfile_s
{
    char *filename; // The name of the file this token file is for
    darr_t tokens;  // Array of token_t
} tokenfile_t;

extern darr_t tokenfiles; // Array of tokenfile_t

void tkn_init(void);

//
// tkn_loadfile
//  - Parse a file into tokens
// Side Affects:
//  - tokenfiles
//
bool tkn_loadfile(FILE* ptr);

#endif