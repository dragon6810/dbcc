#ifndef _lexer_bnf_h
#define _lexer_bnf_h

#include <stdio.h>

#include "darr.h"
#include "bnf.h"

typedef struct tkn_bnf_token_s
{
    bnf_spec_node_t *leaf;
    darr_t parents; // Array of bnf_spec_node_t*
    char *payload;
    unsigned long int iline, icolumn, ichar;
} tkn_bnf_token_t;

typedef struct tkn_bnf_tokenfile_s
{
    darr_t tokens; // Array of tokens
    char *filename;
} tkn_bnf_tokenfile_t;

tkn_bnf_tokenfile_t tkn_bnf_loadfile(char* filename);

#endif