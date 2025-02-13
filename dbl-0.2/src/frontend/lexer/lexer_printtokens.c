#include <frontend/lexer/lexer.h>

#include <assert/assert.h>

void lexer_printtokens(lexer_state_t* state)
{
    int i;

    char type[LEXER_MAXHARDTOKENLEN];

    assert(state);

    for(i=0; i<state->tokens.size; i++)
    {
        lexer_tkntypetostring(state->tokens.data[i].type, type);
        printf("token: \"%s\" (\"%s\") %s:%lu:%lu.\n", state->tokens.data[i].val, type, state->tokens.data[i].file, state->tokens.data[i].line + 1, state->tokens.data[i].col + 1);
    }
}