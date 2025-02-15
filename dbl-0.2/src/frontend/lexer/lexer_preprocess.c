#include <frontend/lexer/lexer.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <assert/assert.h>
#include <cli/cli.h>

void lexer_preprocess_errnofile(lexer_token_t* token)
{
    char path[FILENAME_MAX];

    assert(token);
    
    memcpy(path, token->val + 1, strlen(token->val) - 2);
    path[strlen(token->val) - 2] = 0;

    cli_errorsyntax(token->file, token->line, token->col, "'%s' file not found", path);
    abort();
}

void lexer_preprocess_errnofilename(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];

    cli_errorsyntax(token->file, token->line, token->col + strlen(token->val), "expected \"FILENAME\" or <FILENAME>");
    abort();
}

void lexer_preprocess_errnodirective(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];

    cli_errorsyntax(token->file, token->line, token->col, "invalid preprocessing directive");
    abort();
}

void lexer_preprocess_findinclude(lexer_token_t* token, char* output)
{
    int i;

    char path[FILENAME_MAX], curpath[FILENAME_MAX];

    assert(output);
    assert(token);
    assert(token->type == LEXER_TOKENTYPE_STRING || token->type == LEXER_TOKENTYPE_ANGLESTRING);

    strcpy(path, token->val + 1);
    path[strlen(token->val) - 2] = 0;

    /* first, it its a quote include, check local file */
    if(token->type == LEXER_TOKENTYPE_STRING)
    {
        i = strlen(token->file);
        while(i > 0 && token->file[i] != '/')
            i--;

        memcpy(curpath, token->file, i);
        curpath[i] = 0;
        strcat(curpath, path);

        if(!access(curpath, F_OK))
        {
            strcpy(output, curpath);
            return;
        }
    }

    /* now, look in system include dirs */
    for(i=0; i<cli_includedirs.size; i++)
    {
        strcpy(curpath, cli_includedirs.data[i]);
        if(curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);

        if(access(curpath, F_OK))
            continue;

        strcpy(output, curpath);
        return;
    }

    lexer_preprocess_errnofile(token);
}

void lexer_preprocess_processinclude(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token, *nexttoken;
    char path[FILENAME_MAX];

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];
    nexttoken = &stacktop->tokens.data[itoken+1];

    if((nexttoken->type != LEXER_TOKENTYPE_STRING && nexttoken->type != LEXER_TOKENTYPE_ANGLESTRING) || (token->posline != nexttoken->posline))
        lexer_preprocess_errnofilename(state, itoken);

    lexer_preprocess_findinclude(nexttoken, path);
    puts(path);
}

void lexer_preprocess_processstatement(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken+1];

    switch(token->type)
    {
    case LEXER_TOKENTYPE_INCLUDE:
        lexer_preprocess_processinclude(state, itoken + 1);
        break;
    case LEXER_TOKENTYPE_DEFINE:
        break;
    default:
        lexer_preprocess_errnodirective(state, itoken + 1);
    }
}

void lexer_preprocess_findstatements(lexer_state_t* state)
{
    unsigned long int i;

    lexer_statesrcel_t *stacktop;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    for(i=0; i<stacktop->tokens.size; i++)
    {
        if(stacktop->tokens.data[i].type != LEXER_TOKENTYPE_POUND)
            continue;

        lexer_preprocess_processstatement(state, i);
    }
}

bool lexer_preprocess(lexer_state_t* state)
{
    lexer_preprocess_findstatements(state);

    return true;
}