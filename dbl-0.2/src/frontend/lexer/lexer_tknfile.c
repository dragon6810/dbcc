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

bool lexer_tknfile_eatchar(void)
{
    if(!curchar[0])
    {
        state = LEXER_TOKENTYPE_EOF;
        return true;
    }

    printf("%c", *curchar);

    curchar++;

    return true;
}

bool lexer_tknfile(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    state = -1;
    curchar = srcfile->rawtext;

    while(state != LEXER_TOKENTYPE_EOF)
    {
        if(!lexer_tknfile_eatchar())
            return false;
    }

    puts("");

    return true;
}
