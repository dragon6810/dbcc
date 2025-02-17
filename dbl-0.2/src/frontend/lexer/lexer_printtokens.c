#include <frontend/lexer/lexer.h>

#include <std/assert/assert.h>

void lexer_printtokens(lexer_state_t* state)
{
    int i;

    lexer_statesrcel_t *stacktop;
    char type[LEXER_MAXHARDTOKENLEN];

    assert(state);

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    for(i=0; i<stacktop->tokens.size; i++)
    {
        lexer_tkntypetostring(stacktop->tokens.data[i].type, type);
        printf("token: \"%s\" (\"%s\") %s:%lu:%lu.\n", stacktop->tokens.data[i].val, type, stacktop->tokens.data[i].file, stacktop->tokens.data[i].line + 1, stacktop->tokens.data[i].col + 1);
    }
}