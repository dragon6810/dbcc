#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

lexer_tokentype_e state = -1;
char *curchar = NULL;

bool lexer_tknfile_tknmatches(lexer_tokentype_e type, char* str)
{
    char tknstr[LEXER_MAXHARDTOKENLEN];

    if(!str || strlen(str) < 1)
        return false;

    if(type == LEXER_TOKENTYPE_IDENTIFIER)
        return *str > 32;
    if(type == LEXER_TOKENTYPE_STRING)
        return *str == '"';
    if(type == LEXER_TOKENTYPE_CONSTANT)
        return MATH_INRANGE(*str, '0', '9');

    lexer_tkntypetostring(type, tknstr);
    if(strlen(str) < strlen(tknstr))
        return false;

    return !strncmp(str, tknstr, strlen(tknstr));
}

bool lexer_tknfile(srcfile_t* srcfile)
{
    int i;

    char name[LEXER_MAXHARDTOKENLEN];

    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    state = -1;
    curchar = srcfile->rawtext;
    for(i=LEXER_TOKENTYPE_STARTOFENUM; i<=LEXER_TOKENTYPE_ENDOFENUM; i++)
    {
        if(lexer_tknoneofmany(i, LEXER_TOKENTYPE_EOF, LEXER_TOKENTYPE_STRING, LEXER_TOKENTYPE_IDENTIFIER, LEXER_TOKENTYPE_CONSTANT))
            continue;

        lexer_tkntypetostring(i, name);
        puts(name);
    }

    return true;
}