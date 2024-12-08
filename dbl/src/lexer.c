#include "lexer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>å

#include "error.h"
#include "bnf.h"

darr_t tokenfiles;

void tkn_init(void)
{
    darr_init(&tokenfiles, sizeof(tokenfile_t));
}

bool tkn_loadfile(FILE* ptr)
{
    return true;
}