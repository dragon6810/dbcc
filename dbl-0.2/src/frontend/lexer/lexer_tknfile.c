#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

lexer_tokentype_e state = -1;
char *curchar = NULL;

bool lexer_tknfile_charallowedinidentifier(char c, bool first)
{
    if(MATH_INRANGE(c, 'a', 'z'))
        return true;

    if(MATH_INRANGE(c, 'A', 'Z'))
        return true;

    if(first && MATH_INRANGE(c, '0', '9'))
        return true;

    if(c == '_')
        return true;

    return false;
}

bool lexer_tknfile_isidentifier(char* str)
{
    int i;

    char *strend;
    unsigned long int stringlen;
    char keywordstr[LEXER_MAXHARDTOKENLEN];

    if(!str)
        return false;
    if(!str[0])
        return false;

    if(!lexer_tknfile_charallowedinidentifier(*str, true))
        return false;

    strend = str;
    do
        strend++;
    while(lexer_tknfile_charallowedinidentifier(*strend, false));
    stringlen = strend - str;

    // Make sure it doesn't exactly match a keyword
    for(i=LEXER_TOKENTYPE_STARTOFKEYWORDS; i<=LEXER_TOKENTYPE_ENDOFKEYWORDS; i++)
    {
        lexer_tkntypetostring(i, keywordstr);
        if(strlen(keywordstr) != stringlen)
            continue;

        if(!strncmp(keywordstr, str, stringlen))
            return false;
    }

    return true;
}

bool lexer_tknfile_isconstant(char* str)
{
    char *strend;
    unsigned long int stringlen;
    bool encountereddot;

    if(!str)
        return false;
    if(!str[0])
        return false;

    strend = str;
    encountereddot = finished = false;
    while(true)
    {
        if(!strend[0])
            break;

        if(strend[0] == '.')
        {
            if(encountereddot)
                return false;

            encountereddot = true;
            continue;
        }

        if(strend[0] == 'f' || strend[0] == 'F' || strend[0] == 'd' || strend[0] == 'D')
        {
            strend++;
            break;
        }

        if(!MATH_INRANGE(strend[0], '0', '9'))
            break;

        strend++;
    }
    stringlen = strend - str;

    if(!stringlen)
        return false;

    return true;
}

bool lexer_tknfile_tknmatches(lexer_tokentype_e type, char* str)
{
    char tknstr[LEXER_MAXHARDTOKENLEN];

    if(!str || strlen(str) < 1)
        return false;

    if(type == LEXER_TOKENTYPE_IDENTIFIER)
        return lexer_tknfile_isidentifier(str);
    if(type == LEXER_TOKENTYPE_STRING)
        return *str == '"';
    if(type == LEXER_TOKENTYPE_CONSTANT)
        return lexer_tknfile_isconstant(str);

    lexer_tkntypetostring(type, tknstr);
    if(strlen(str) < strlen(tknstr))
        return false;

    if(strncmp(str, tknstr, strlen(tknstr)))
        return false;

    return true;
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
