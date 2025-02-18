#include <frontend/lexer/lexer.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cli/cli.h>
#include <std/assert/assert.h>
#include <std/math/math.h>
#include <std/profiler/profiler.h>

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

void lexer_preprocess_errnomacro(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];

    cli_errorsyntax(token->file, token->line, token->col + strlen(token->val), "macro name missing");
    abort();
}

void lexer_preprocess_errmacroident(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];

    cli_errorsyntax(token->file, token->line, token->col, "macro name must be an identifier");
    abort();
}

void lexer_preprocess_errdirectiveextratokens(lexer_state_t* state, unsigned long int itoken, const char* directive)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];

    cli_errorsyntax(token->file, token->line, token->col, "extra tokens at end of %s directive", directive);
    abort();
}

void lexer_preprocess_excludeconditional(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    unsigned long int depth, begin, end;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    depth = 1;
    begin = itoken - 1;
    end = itoken + 1;

    while(depth && stacktop->tokens.data[end].type != LEXER_TOKENTYPE_EOF)
    {
        if(stacktop->tokens.data[end].type == LEXER_TOKENTYPE_ENDIF)
            depth--;

        if(MATH_INRANGE(stacktop->tokens.data[end].type, LEXER_TOKENTYPE_STARTOFPREPROCCOND, LEXER_TOKENTYPE_ENDOFPREPROCCOND))
            depth++;

        end++;
    }

    LIST_REMOVERANGE(stacktop->tokens, begin, end);
}

void lexer_preprocess_includeconditional(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    unsigned long int depth, begin, end;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    depth = 1;
    begin = itoken - 1;
    end = itoken + 1;

    while(depth && stacktop->tokens.data[end].type != LEXER_TOKENTYPE_EOF)
    {
        if(stacktop->tokens.data[end].type == LEXER_TOKENTYPE_ENDIF)
            depth--;

        if(MATH_INRANGE(stacktop->tokens.data[end].type, LEXER_TOKENTYPE_STARTOFPREPROCCOND, LEXER_TOKENTYPE_ENDOFPREPROCCOND))
            depth++;

        end++;
    }
    LIST_REMOVERANGE(stacktop->tokens, end - 2, end);

    end = begin;
    while(stacktop->tokens.data[end].type != LEXER_TOKENTYPE_EOF && stacktop->tokens.data[end].posline == stacktop->tokens.data[begin].posline)
        end++;

    LIST_REMOVERANGE(stacktop->tokens, begin, end);
}

void lexer_preprocess_processifdef(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token, *next;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];
    next = &stacktop->tokens.data[itoken+1];

    if(next->type != LEXER_TOKENTYPE_IDENTIFIER && !MATH_INRANGE(next->type, LEXER_TOKENTYPE_STARTOFKEYWORDS, LEXER_TOKENTYPE_ENDOFKEYWORDS))
        lexer_preprocess_errmacroident(state, itoken + 1);

    if(token->posline != next->posline)
        lexer_preprocess_errnomacro(state, itoken);

    if(next->posline == (next+1)->posline)
        lexer_preprocess_errdirectiveextratokens(state, itoken+2, "#ifdef");

    if(HASHMAP_FETCH(state->defines, next->val))
        lexer_preprocess_includeconditional(state, itoken);
    else
        lexer_preprocess_excludeconditional(state, itoken);
}

bool lexer_preprocess_tryreplacedefine(lexer_state_t* state, unsigned long int itoken)
{
    unsigned long int i;

    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;
    lexer_define_t *define;
    list_lexer_token_t newtokens;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];
    token = &stacktop->tokens.data[itoken];

    if(!MATH_INRANGE(token->type, LEXER_TOKENTYPE_STARTOFKEYWORDS, LEXER_TOKENTYPE_ENDOFKEYWORDS) && token->type != LEXER_TOKENTYPE_IDENTIFIER)
        return false;

    define = HASHMAP_FETCH(state->defines, token->val);
    if(!define)
        return false;

    LIST_INITIALIZE(newtokens);
    LIST_RESIZE(newtokens, define->tokens.size);
    for(i=0; i<newtokens.size; i++)
    {
        memcpy(&newtokens.data[i], &define->tokens.data[i], sizeof(lexer_token_t));
        newtokens.data[i].val = strdup(newtokens.data[i].val);
        newtokens.data[i].line = stacktop->tokens.data[itoken].line;
        newtokens.data[i].col = stacktop->tokens.data[itoken].col;
        newtokens.data[i].posline = stacktop->tokens.data[itoken].posline;
    }

    free(stacktop->tokens.data[itoken].val);
    LIST_REMOVE(stacktop->tokens, itoken);
    LIST_INSERTLIST(stacktop->tokens, newtokens, itoken);

    LIST_FREE(newtokens);

    return true;
}

void lexer_preprocess_processdefine(lexer_state_t* state, unsigned long int itoken)
{
    unsigned long int i;

    lexer_statesrcel_t *stacktop;
    unsigned long int lasttoken;
    lexer_define_t define;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    for(lasttoken=itoken; stacktop->tokens.data[itoken].posline == stacktop->tokens.data[lasttoken].posline; lasttoken++);

    define.name = strdup(stacktop->tokens.data[itoken].val);
    LIST_INITIALIZE(define.tokens);
    LIST_RESIZE(define.tokens, lasttoken-(itoken+2));
    for(i=itoken+2; i<lasttoken; i++)
    {
        memcpy(&define.tokens.data[i-(itoken+2)], &stacktop->tokens.data[i], sizeof(lexer_token_t));
        define.tokens.data[i-(itoken+2)].val = strdup(define.tokens.data[i-(itoken+2)].val);
    }

    HASHMAP_SET(state->defines, stacktop->tokens.data[itoken+1].val, define);

    LIST_REMOVERANGE(stacktop->tokens, itoken - 1, lasttoken);
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
    unsigned long int istacktop;
    lexer_statesrcel_t *stacktop, newtop;
    lexer_token_t *token, *nexttoken;
    char path[FILENAME_MAX];

    istacktop = state->srcstack.size - 1;
    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken];
    nexttoken = &stacktop->tokens.data[itoken+1];

    if((nexttoken->type != LEXER_TOKENTYPE_STRING && nexttoken->type != LEXER_TOKENTYPE_ANGLESTRING) || (token->posline != nexttoken->posline))
        lexer_preprocess_errnofilename(state, itoken);

    lexer_preprocess_findinclude(nexttoken, path);
    strcpy(newtop.filename, path);
    newtop.curline = newtop.curcolumn = 1;
    LIST_INITIALIZE(newtop.lines);
    LIST_INITIALIZE(newtop.tokens);
    LIST_PUSH(state->srcstack, newtop);
    lexer_initialprocessing(state);
    lexer_tokenize(state);
    LIST_POP(state->srcstack.data[state->srcstack.size-1].tokens, NULL); /* remove eof */

    stacktop = &state->srcstack.data[istacktop];

    LIST_REMOVERANGE(stacktop->tokens, itoken - 1, itoken + 2);
    LIST_INSERTLIST(stacktop->tokens, state->srcstack.data[state->srcstack.size-1].tokens, itoken - 1);

    LIST_POP(state->srcstack, NULL);
}

bool lexer_preprocess_processstatement(lexer_state_t* state, unsigned long int itoken)
{
    lexer_statesrcel_t *stacktop;
    lexer_token_t *token;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    token = &stacktop->tokens.data[itoken+1];

    switch(token->type)
    {
    case LEXER_TOKENTYPE_INCLUDE:
        lexer_preprocess_processinclude(state, itoken + 1);
        return true;
    case LEXER_TOKENTYPE_DEFINE:
        lexer_preprocess_processdefine(state, itoken + 1);
        return true;
    case LEXER_TOKENTYPE_IFDEF:
        lexer_preprocess_processifdef(state, itoken + 1);
        return true;
    default:
        lexer_preprocess_errnodirective(state, itoken + 1);
        return false;
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
        {
            if(lexer_preprocess_tryreplacedefine(state, i))
                i--;

            continue;
        }

        if(lexer_preprocess_processstatement(state, i))
            i--;
        stacktop = &state->srcstack.data[state->srcstack.size - 1];
    }
}

bool lexer_preprocess(lexer_state_t* state)
{
    profiler_push("Lex File: Preprocessing");

    lexer_preprocess_findstatements(state);

    profiler_pop();

    return true;
}