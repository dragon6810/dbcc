#include <frontend/lexer/lexer.h>

#include <stdio.h>
#include <string.h>

#include <cli/cli.h>
#include <std/assert/assert.h>
#include <std/math/math.h>
#include <std/profiler/profiler.h>
#include <textutils/textutils.h>

static bool lexer_tokenize_ischarallowedinconstant(char c, bool hex)
{
    if(MATH_INRANGE(c, '0', '9'))
        return true;
    if(hex && MATH_INRANGE(c, 'a', 'f'))
        return true;
    if(hex && MATH_INRANGE(c, 'A', 'F'))
        return true;

    if(c == '.' || c == 'f' || c == 'F')
        return true;

    return false;
}

static int lexer_tokenize_isconstant(char* str)
{
    char *beginning;
    bool hex;

    if(!str)
        return 0;
    if(!*str)
        return 0;

    hex = false;
    if(strlen(str) >= 2 && (!strncmp(str, "0x", 2) || !strncmp(str, "0X", 2)))
    {
        hex = true;
        str += 2;
    }

    beginning = str;
    while(lexer_tokenize_ischarallowedinconstant(*str, hex))
        str++;

    return str - beginning;
}

static bool lexer_tokenize_ischarallowedinidentifier(char c, bool first)
{
    if(MATH_INRANGE(c, 'a', 'z'))
        return true;

    if(MATH_INRANGE(c, 'A', 'Z'))
        return true;

    if(!first && MATH_INRANGE(c, '0', '9'))
        return true;

    if(c == '_' || c == '$')
        return true;

    return false;
}

static int lexer_tokenize_isidentifier(char* str)
{
    char *beginning;

    if(!str)
        return 0;
    if(!*str)
        return 0;

    beginning = str;
    while(lexer_tokenize_ischarallowedinidentifier(*str, str == beginning))
        str++;

    return str - beginning;
}

static int lexer_tokenize_ischarconstant(char* str)
{
    char *beginning;

    if(!str)
        return 0;
    if(strlen(str) < 3)
        return 0;
    if(*str != '\'')
        return 0;

    beginning = str;
    do
        str++;
    while(*str && (*str != '\'' || textutils_ischarcancelled(beginning, str)));
    
    if(!*str || !*str++)
        return 0;

    if(str - beginning > 4)
        return 0;

    if((str - beginning == 4) && (*(beginning + 1) != '\\'))
        return 0;

    if((str - beginning == 3) && (*(beginning + 1) == '\\'))
        return 0;
    
    if(str - beginning < 3)
        return 0;

    return str - beginning;
}

static int lexer_tokenize_isanglestring(char* str)
{
    char *beginning;

    if(!str)
        return 0;
    if(strlen(str) < 2)
        return 0;
    if(*str != '<')
        return 0;

    beginning = str;
    while(*str && (*str != '>' || textutils_ischarcancelled(beginning, str)))
        str++;
    
    if(!*str)
        return 0;

    return str - beginning;
}

static int lexer_tokenize_isstring(char* str)
{
    char *beginning;

    if(!str)
        return 0;
    if(strlen(str) < 2)
        return 0;
    if(*str != '"')
        return 0;

    beginning = str;
    do
        str++;
    while(*str && (*str != '"' || textutils_ischarcancelled(beginning, str)));
    
    if(!*str || !*str++)
        return 0;

    return str - beginning;
}

static int lexer_tokenize_doestokenmatch(char* str, lexer_tokentype_e type)
{
    char tknstr[LEXER_MAXHARDTOKENLEN];

    if(!str || strlen(str) < 1)
        return 0;

    if(type == LEXER_TOKENTYPE_STRING)
        return lexer_tokenize_isstring(str);
    if(type == LEXER_TOKENTYPE_ANGLESTRING)
        return lexer_tokenize_isanglestring(str);
    if(type == LEXER_TOKENTYPE_CHARCONSTANT)
        return lexer_tokenize_ischarconstant(str);
    if(type == LEXER_TOKENTYPE_IDENTIFIER)
        return lexer_tokenize_isidentifier(str);
    if(type == LEXER_TOKENTYPE_CONSTANT)
        return lexer_tokenize_isconstant(str);

    lexer_tkntypetostring(type, tknstr);
    if(strlen(str) < strlen(tknstr))
        return 0;

    if(strncmp(str, tknstr, strlen(tknstr)))
        return 0;

    return strlen(tknstr);
}

static void lexer_tokenize_errnotoken(lexer_state_t* state, unsigned long int line, unsigned long int column)
{
    lexer_statesrcel_t *stacktop;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    cli_errorsyntax(stacktop->filename, line, column, "unknown token");
    abort();
}

static lexer_token_t lexer_tokenize_findtoken(lexer_state_t* state, unsigned long int line, unsigned long int column)
{
    int i;

    lexer_token_t token;
    lexer_statesrcel_t *stacktop;
    unsigned long int longlen, curlen;
    lexer_tokentype_e longmatch;
    list_lexer_barrier_t *barriers;
    lexer_barrier_t *latestbarrier;
    char *linestr;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    assert(line < stacktop->lines.size);
    linestr = stacktop->lines.data[line].str;
    assert(linestr);
    assert(column < strlen(linestr));

    barriers = &stacktop->lines.data[line].barriers;
    latestbarrier = &stacktop->lines.data[line].barriers.data[0];
    while((latestbarrier - barriers->data) < barriers->size - 1 && latestbarrier->position < column)
        latestbarrier++;
    if(latestbarrier->position > column && (latestbarrier - barriers->data))
        latestbarrier--;

    longlen = longmatch = 0;
    for(i=LEXER_TOKENTYPE_STARTOFENUM; i<=LEXER_TOKENTYPE_ENDOFENUM; i++)
    {
        curlen = lexer_tokenize_doestokenmatch(linestr + column, i);
        if(curlen <= longlen)
            continue;

        longlen = curlen;
        longmatch = i;
    }

    if(!longlen)
        lexer_tokenize_errnotoken(state, latestbarrier->line, column - latestbarrier->position + latestbarrier->column);

    token.val = malloc(longlen + 1);
    memcpy(token.val, linestr + column, longlen);
    token.val[longlen] = 0;
    token.type = longmatch;
    strcpy(token.file, stacktop->filename);
    token.line = latestbarrier->line;
    token.col = column - latestbarrier->position + latestbarrier->column;
    token.posline = line;

    return token;
}

static bool lexer_tokenize_tokenizeline(lexer_state_t* state, unsigned long int line)
{
    lexer_statesrcel_t *stacktop;
    lexer_line_t *pline;
    lexer_token_t token;
    char type[LEXER_MAXHARDTOKENLEN];
    unsigned long int column, linelen;

    stacktop = &state->srcstack.data[state->srcstack.size - 1];
    assert(line < stacktop->lines.size);
    pline = &stacktop->lines.data[line];
    linelen = strlen(pline->str);

    column = 0;
    while(column < linelen && pline->str[column] <= 32)
            column++;

    while(column < linelen)
    {
        token = lexer_tokenize_findtoken(state, line, column);
        lexer_tkntypetostring(token.type, type);
        column += strlen(token.val);

        LIST_PUSH(stacktop->tokens, token);
    
        while(column < linelen && pline->str[column] <= 32)
            column++;
    }

    return true;
}

static void lexer_tokenize_appendeof(lexer_state_t* state)
{
    unsigned long int pos, column;
    lexer_statesrcel_t *stacktop;
    lexer_line_t *lastline;
    lexer_barrier_t *lastbarrier;
    lexer_token_t eof;

    assert(state);
    
    stacktop = &state->srcstack.data[state->srcstack.size - 1];
    lastline = &stacktop->lines.data[stacktop->lines.size - 1];
    lastbarrier = &lastline->barriers.data[lastline->barriers.size - 1];

    pos = strlen(lastline->str);
    column = pos - lastbarrier->position + lastbarrier->column;

    eof.val = calloc(1, 1);
    eof.type = LEXER_TOKENTYPE_EOF;
    strcpy(eof.file, stacktop->filename);
    eof.line = lastbarrier->line;
    eof.col = column;
    eof.posline = stacktop->lines.size - 1;

    LIST_PUSH(stacktop->tokens, eof);
}

bool lexer_tokenize(lexer_state_t* state)
{
    int i;

    lexer_statesrcel_t *stacktop;

    profiler_push("Lex File: Tokenize");

    stacktop = &state->srcstack.data[state->srcstack.size - 1];

    for(i=0; i<stacktop->lines.size; i++)
        lexer_tokenize_tokenizeline(state, i);

    lexer_tokenize_appendeof(state);

    profiler_pop();

    return true;
}