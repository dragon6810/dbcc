#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <cli/cli.h>
#include <list/list.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

#define LEXER_TKNFILE_COMMENTONELINER 1
#define LEXER_TKNFILE_COMMENTCROSSLINE 2

typedef struct lexer_tknfile_define_s lexer_tknfile_define_t;

struct lexer_tknfile_define_s
{
    char *key, *val;
};

lexer_tokentype_e state = LEXER_TOKENTYPE_INVALID;
char *curchar = NULL, *rawtext = NULL;
unsigned long int stateprogress = 0;
int incomment = 0; 
char *filename = NULL;
int line, col = 1;
list_t defines;
list_t tokens;

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

int lexer_tknfile_isstring(char* str)
{
    char *strend;

    if(!str)
        return 0;
    if(str[0] != '"')
        return 0;

    strend = str;
    do
    {
        strend++;
    } while(*strend != '"' || lexer_tknfile_ischarcancelled(rawtext, strend));

    return strend - str + 1;
}

int lexer_tknfile_ischaracter(char* str)
{

    char *strend;

    if(!str)
        return 0;
    if(str[0] != '\'')
        return 0;

    strend = str;
    do
    {
        if(*strend == 0)
        {
            printf("\x1B[31merror in %s: \x1B[0mchar constant not terminated at %d:%d.\n", filename, line, col); 
            abort();
        }

        strend++;
    } while(*strend != '\'' || lexer_tknfile_ischarcancelled(rawtext, strend));

    if(((strend-str) != 4 && str[1] == '\'') || ((strend-str) != 3 && str[1] != '\''))
    {
        printf("\x1B[31merror in %s: \x1B[0mchar constant wrong length at %d:%d.\n", filename, line, col); 
        abort();
    }

    return strend - str + 1;
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

int lexer_tknfile_tknmatches(lexer_tokentype_e type, char* str)
{
    char tknstr[LEXER_MAXHARDTOKENLEN];

    if(!str || strlen(str) < 1)
        return 0;

    if(type == LEXER_TOKENTYPE_IDENTIFIER)
        return lexer_tknfile_isidentifier(str);
    if(type == LEXER_TOKENTYPE_STRING)
        return lexer_tknfile_isstring(str);
    if(type == LEXER_TOKENTYPE_CHARCONSTANT)
        return lexer_tknfile_ischaracter(str);
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
    if(!str)
        return str;

    while(*str <= 32 && *str)
    {
        col++;
        if(*str == '\n')
        {
            line++;
            col = 1;
        }
        str++;
    }

    return str;
}

void lexer_tknfile_replacesubstr(char* newstr, int replen)
{
    char *newrawtext;
    unsigned long int newlen, charoffs;

    assert(newstr);
    assert(replen <= strlen(curchar));

    newlen = strlen(rawtext) - replen + strlen(newstr);
    newrawtext = malloc(newlen + 1);

    charoffs = curchar - rawtext;
    
    memcpy(newrawtext, rawtext, charoffs);
    newrawtext[charoffs] = 0;
    strcat(newrawtext, newstr);
    strcat(newrawtext, curchar + replen);

    free(rawtext);
    rawtext = newrawtext;
    curchar = rawtext + charoffs;
}

bool lexer_tknfile_tryreplacedefine(int tokenlen)
{
    int i;

    lexer_tknfile_define_t *definesdata;

    definesdata = (lexer_tknfile_define_t*) defines.data;
    for(i=defines.size-1; i>=0; i--)
    {
        if(strncmp(definesdata[i].key, curchar, tokenlen))
            continue;

        lexer_tknfile_replacesubstr(definesdata[i].val, tokenlen);

        return true;
    }

    return false;
}

void lexer_tknfile_processdefine(void)
{
    char *start, *end;
    lexer_tknfile_define_t define;

    curchar = lexer_tknfile_skipwhitespace(curchar);

    if(!*curchar)
        return;

    start = end = curchar;
    do
    {
        end++;
    } while(lexer_tknfile_charallowedinidentifier(*end, end == start+1));

    define.key = malloc(end - start + 1);
    memcpy(define.key, start, end - start);
    define.key[end - start] = 0;

    col += end - curchar;
    curchar = end;
    curchar = lexer_tknfile_skipwhitespace(curchar);

    if(!*curchar)
    {
        free(define.key);
        return;
    }

    start = end = curchar;
    do
    {
        end++;
    } while(*end != '\n' && !lexer_tknfile_ischarcancelled(rawtext, end));

    define.val = malloc(end - start + 1);
    memcpy(define.val, start, end - start);
    define.val[end - start] = 0;

    col += end - curchar;
    curchar = end;
    curchar = lexer_tknfile_skipwhitespace(curchar);

    list_push(&defines, &define);
}

void lexer_tknfile_processdirective(void)
{
    if(*curchar != '#')
        return;

    curchar++;
    col++;
    if(!strncmp(curchar, "define", strlen("define")))
    {
        col += strlen("define");
        curchar += strlen("define");
        return lexer_tknfile_processdefine();
    }
}

void lexer_tknfile_findnexttkn(void)
{
    int i;

    int len;
    char str[LEXER_MAXHARDTOKENLEN];
    int longestmatch, longestlen;
    char *tokenval;
    lexer_token_t newtkn;

    if(!*curchar)
        return;

    if(*curchar == '#')
    {
        lexer_tknfile_processdirective();
        curchar = lexer_tknfile_skipwhitespace(curchar);
        state = LEXER_TOKENTYPE_INVALID;
        stateprogress = 0;
        lexer_tknfile_findnexttkn();
        return;
    }

    longestmatch = -1;
    longestlen = 0;
    for(i=LEXER_TOKENTYPE_STARTOFENUM; i<=LEXER_TOKENTYPE_ENDOFENUM; i++)
    {
        if(!(len = lexer_tknfile_tknmatches(i, curchar)))
            continue;
        if(len <= longestlen)
            continue;

        longestmatch = i;
        longestlen = len;
    }

    if(!longestlen)
    {
        printf("\x1B[31merror in %s: \x1B[0minvalid token start %c at %d:%d.\n", filename, *curchar, line, col); 
        abort();
    }

    if(longestmatch == LEXER_TOKENTYPE_IDENTIFIER)
    {
        if(lexer_tknfile_tryreplacedefine(longestlen))
        {
            state = LEXER_TOKENTYPE_INVALID;
            stateprogress = 0;
            lexer_tknfile_findnexttkn();
            return;
        }
    }

    lexer_tkntypetostring(longestmatch, str);
    state = longestmatch;
    stateprogress = longestlen;
    tokenval = malloc(longestlen + 1);
    memcpy(tokenval, curchar, longestlen);
    tokenval[longestlen] = 0;

    newtkn.line = line;
    newtkn.col = col;
    newtkn.file = filename;
    newtkn.type = longestmatch;
    newtkn.val = tokenval;

    list_push(&tokens, &newtkn);

    if(cli_verbose)
        printf("token \"%s\" (\"%s\") at %s:%d:%d.\n", tokenval, str, filename, line, col);
}

void lexer_tknfile_updatecomment(void)
{
    if(!curchar || !*curchar)
        return;

    if(incomment == LEXER_TKNFILE_COMMENTONELINER && *curchar == '\n')
    {
        incomment = 0;
        line++;
        col = 1;
        curchar++;
        return;
    }

    if(strlen(curchar) < 2)
        return;

    if(incomment == LEXER_TKNFILE_COMMENTCROSSLINE && !strncmp("*/", curchar, 2))
    {
        incomment = 0;
        curchar += 2;
        return;
    }

    if(!strncmp("//", curchar, 2))
    {
        state = LEXER_TOKENTYPE_INVALID;
        stateprogress = 0;
        incomment = LEXER_TKNFILE_COMMENTONELINER;
        curchar += 2;
        return;
    }

    if(!strncmp("/*", curchar, 2))
    {
        state = LEXER_TOKENTYPE_INVALID;
        stateprogress = 0;
        incomment = LEXER_TKNFILE_COMMENTCROSSLINE;
        curchar += 2;
        return;
    }
}

bool lexer_tknfile_eatchar(void)
{
    if(!curchar || !curchar[0])
    {
        state = LEXER_TOKENTYPE_EOF;
        return true;
    }

    if(!incomment)
        curchar = lexer_tknfile_skipwhitespace(curchar);
    lexer_tknfile_updatecomment();
    if(!incomment)
        curchar = lexer_tknfile_skipwhitespace(curchar);

    if(!stateprogress && !incomment)
    {
        if(!*curchar)
        {
            state = LEXER_TOKENTYPE_EOF;
            return true;
        }
        lexer_tknfile_findnexttkn();
    }

    if(!curchar[0])
    {
        state = LEXER_TOKENTYPE_EOF;
        return true;
    }

    col++;
    if(*curchar == '\n')
    {
        col = 1;
        line++;
    }
    curchar++;
    if(!incomment)
        stateprogress--;

    return true;
}

bool lexer_tknfile(srcfile_t* srcfile)
{
    int i;

    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    state = LEXER_TOKENTYPE_INVALID;
    rawtext = curchar = strdup(srcfile->rawtext);
    incomment = 0;
    stateprogress = 0;
    filename = srcfile->path;
    line = col = 1; 
    list_initialize(&defines, sizeof(lexer_tknfile_define_t));
    list_initialize(&tokens, sizeof(lexer_token_t));

    while(state != LEXER_TOKENTYPE_EOF)
    {
        if(!lexer_tknfile_eatchar())
            return false;
    }

    srcfile->tokens = tokens;

    for(i=0; i<defines.size; i++)
    {
        free(((lexer_tknfile_define_t*)defines.data)[i].key);
        free(((lexer_tknfile_define_t*)defines.data)[i].val); 
    }
    list_free(&defines);

    free(rawtext);

    return true;
}
