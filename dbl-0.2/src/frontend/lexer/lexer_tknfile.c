#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

lexer_tokentype_e state = LEXER_TOKENTYPE_INVALID;
char *curchar = NULL;
unsigned long int stateprogress = 0;

bool lexer_tknfile_charallowedinidentifier(char c, bool first)
{
    if(MATH_INRANGE(c, 'a', 'z'))
        return true;

    if(MATH_INRANGE(c, 'A', 'Z'))
        return true;

    if(!first && MATH_INRANGE(c, '0', '9'))
        return true;

    if(c == '_')
        return true;

    return false;
}

int lexer_tknfile_isidentifier(char* str)
{
    int i;

    char *strend;
    unsigned long int stringlen;
    char keywordstr[LEXER_MAXHARDTOKENLEN];

    if(!str)
        return 0;
    if(!str[0])
        return 0;

    if(!lexer_tknfile_charallowedinidentifier(*str, true))
        return 0;

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
            return 0;
    }

    return stringlen;
}

int lexer_tknfile_isconstant(char* str)
{
    char *strend;
    unsigned long int stringlen;
    bool encountereddot;

    if(!str)
        return 0;
    if(!str[0])
        return 0;

    strend = str;
    encountereddot = false;
    while(true)
    {
        if(!strend[0])
            break;

        if(strend[0] == '.')
        {
            if(encountereddot)
                return 0;

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

    return stringlen;
}

bool lexer_tknfile_ischarcancelled(char* str, char* c)
{
    int i;

    bool canceled;

    i = 0;
    canceled = false;
    do
    {
        c--;

        if(*c != '\\' || i > 1)
            break;
    
        canceled = !canceled;
    
        i++;
    } while(c > str);
    
    return canceled;
}

int lexer_tknfile_tknmatches(lexer_tokentype_e type, char* str)
{
    char tknstr[LEXER_MAXHARDTOKENLEN];

    if(!str || strlen(str) < 1)
        return 0;

    if(type == LEXER_TOKENTYPE_IDENTIFIER)
        return lexer_tknfile_isidentifier(str);
    if(type == LEXER_TOKENTYPE_STRING)
        return *str == '"';
    if(type == LEXER_TOKENTYPE_CONSTANT)
        return lexer_tknfile_isconstant(str);

    lexer_tkntypetostring(type, tknstr);
    if(strlen(str) < strlen(tknstr))
        return 0;

    if(strncmp(str, tknstr, strlen(tknstr)))
        return 0;

    return strlen(tknstr);
}

char* lexer_tknfile_skipwhitespace(char* str)
{
    if(!str || !*str)
        return str;

    while(*str <= 32 && *str)
        str++;

    return str;
}

void lexer_tknfile_findnexttkn(void)
{
    int i;

    int len;
    char str[LEXER_MAXHARDTOKENLEN];

    for(i=LEXER_TOKENTYPE_STARTOFENUM; i<=LEXER_TOKENTYPE_ENDOFENUM; i++)
    {
        if(!(len = lexer_tknfile_tknmatches(i, curchar)))
            continue;
        
        lexer_tkntypetostring(i, str);
        stateprogress = len;
        puts(str);

        return;
    }
}

bool lexer_tknfile_eatchar(void)
{
    if(!curchar[0])
    {
        state = LEXER_TOKENTYPE_EOF;
        return true;
    }

    if(!stateprogress)
    {
        curchar = lexer_tknfile_skipwhitespace(curchar);
        lexer_tknfile_findnexttkn();
    }

    curchar++;
    stateprogress--;

    return true;
}

bool lexer_tknfile(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    state = LEXER_TOKENTYPE_INVALID;
    curchar = srcfile->rawtext;
    stateprogress = 0;

    while(state != LEXER_TOKENTYPE_EOF)
    {
        if(!lexer_tknfile_eatchar())
            return false;
    }

    puts("");

    return true;
}
