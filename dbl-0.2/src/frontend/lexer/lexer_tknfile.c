#include <frontend/lexer/lexer.h>

#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <assert/assert.h>
#include <cli/cli.h>
#include <list/list.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

#define LEXER_TKNFILE_COMMENTONELINER 1
#define LEXER_TKNFILE_COMMENTCROSSLINE 2

typedef struct lexer_tknfile_define_s lexer_tknfile_define_t;
typedef struct lexer_tknfile_incstackel_s lexer_tknfile_incstackel_t;

struct lexer_tknfile_define_s
{
    char *key, *val;
};

struct lexer_tknfile_incstackel_s
{
    unsigned long int start, end;
    char filename[PATH_MAX];
    unsigned long int line, column;
};

lexer_tokentype_e state = LEXER_TOKENTYPE_INVALID;
char *curchar = NULL, *rawtext = NULL;
unsigned long int stateprogress = 0;
int incomment = 0; 
list_t defines;
list_t tokens;
list_t lexer_tknfile_incstack;

void lexer_tknfile_advcurchar(void)
{
    lexer_tknfile_incstackel_t *inctop;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    inctop->column++;
    if(*curchar == '\n')
    {
        inctop->line++;
        inctop->column = 1;
    }

    curchar++;

    if(curchar - rawtext > inctop->end)
        list_pop(&lexer_tknfile_incstack, NULL);
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
        strend++;
    while(*strend != '"' || lexer_tknfile_ischarcancelled(rawtext, strend));

    return strend - str + 1;
}

int lexer_tknfile_ischaracter(char* str)
{
    lexer_tknfile_incstackel_t *inctop;
    char *strend;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    if(!str)
        return 0;
    if(str[0] != '\'')
        return 0;

    strend = str;
    do
    {
        if(*strend == 0)
        {
            fprintf(stderr, "\x1B[31merror in %s: \x1B[0mchar constant not terminated at %lu:%lu.\n", inctop->filename, inctop->line, inctop->column); 
            abort();
        }

        strend++;
    } while(*strend != '\'' || lexer_tknfile_ischarcancelled(rawtext, strend));

    if(((strend-str) != 4 && str[1] == '\'') || ((strend-str) != 3 && str[1] != '\''))
    {
        fprintf(stderr, "\x1B[31merror in %s: \x1B[0mchar constant wrong length at %lu:%lu.\n", inctop->filename, inctop->line, inctop->column); 
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

void lexer_tknfile_skipwhitespace()
{
    if(!curchar)
        return;

    while(*curchar <= 32 && *curchar)
        lexer_tknfile_advcurchar();
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

void lexer_tknfile_processinclude(void)
{
    lexer_tknfile_incstackel_t *inctop;
    FILE *ptr;
    bool local;
    char *start, *end, *stateend;
    char *path, *dir, *realpath;
    char *filecontents;
    unsigned long int contentslen;
    lexer_tknfile_incstackel_t newtop;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    lexer_tknfile_skipwhitespace();

    if(*curchar != '"' && *curchar != '<')
    {
        fprintf(stderr, "\x1B[31merror in %s: \x1B[0m expected < or \" after include but got %c %lu:%lu.\n", inctop->filename, *curchar, inctop->line, inctop->column); 
        abort();
    }

    local = false;
    if(*curchar == '"')
    {
        local = true;
        start = end = curchar + 1;
        while(*end != '"' && *end && *end != '\n')
            end++;
    }
    else
    {
        start = end = curchar + 1;
    }

    if(*end != '"')
    {
        fprintf(stderr, "\x1B[31merror in %s: \x1B[0mexpected \" to terminate include path %lu:%lu.\n", inctop->filename, inctop->line, inctop->column); 
        abort();
    }

    stateend = end;

    path = malloc(end - start + 1);
    memcpy(path, start, end - start);
    path[end - start] = 0;

    if(local)
    {
        start = inctop->filename;
        end = inctop->filename + strlen(inctop->filename);
        do
            end--;
        while(*end != '/' && end > start);

        if(end - start == 0)
        {
            dir = NULL;
            realpath = strdup(path);
        }
        else
        {
            dir = malloc(end - start + 1);
            memcpy(dir, start, end - start);
            dir[end - start] = 0;

            realpath = malloc(strlen(dir) + strlen(path) + 1);
            strcpy(realpath, dir);
            strcat(realpath, path);
        }
    }
    else
    {
        dir = 0;
    }

    if(access(realpath, F_OK))
    {
        fprintf(stderr, "\x1B[31merror in %s: \x1B[0minclude file %s not found, included at %lu:%lu.\n", inctop->filename, path, inctop->line, inctop->column); 
        abort();
    }

    ptr = fopen(realpath, "r");
    fseek(ptr, 0, SEEK_END);
    filecontents = malloc((contentslen = ftell(ptr)) + 1);
    fseek(ptr, 0, SEEK_SET);
    fread(filecontents, 1, contentslen, ptr);
    filecontents[contentslen] = 0;

    inctop->column += (stateend + 1) - curchar;
    curchar = stateend + 1;

    newtop.line = newtop.column = 1;
    strcpy(newtop.filename, realpath);
    newtop.start = curchar - rawtext;
    newtop.end = newtop.start + contentslen;
    
    inctop->line++;
    for(; inctop>=(lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data; inctop--)
        inctop->end += contentslen;

    list_push(&lexer_tknfile_incstack, &newtop);
    lexer_tknfile_replacesubstr(filecontents, 0);

    free(path);
    if(dir)
        free(dir);
    free(realpath);
    free(filecontents);
    fclose(ptr);

    lexer_tknfile_skipwhitespace();
}

void lexer_tknfile_processdefine(void)
{
    lexer_tknfile_incstackel_t *inctop;
    char *start, *end;
    lexer_tknfile_define_t define;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    lexer_tknfile_skipwhitespace();

    if(!*curchar)
        return;

    start = end = curchar;
    do
        end++;
    while(lexer_tknfile_charallowedinidentifier(*end, end == start+1));

    define.key = malloc(end - start + 1);
    memcpy(define.key, start, end - start);
    define.key[end - start] = 0;

    inctop->column += end - curchar;
    curchar = end;
    lexer_tknfile_skipwhitespace();

    if(!*curchar)
    {
        free(define.key);
        return;
    }

    start = end = curchar;
    do
        end++;
    while(*end != '\n' && !lexer_tknfile_ischarcancelled(rawtext, end));

    define.val = malloc(end - start + 1);
    memcpy(define.val, start, end - start);
    define.val[end - start] = 0;

    inctop->column += end - curchar;
    curchar = end;
    lexer_tknfile_skipwhitespace();

    list_push(&defines, &define);
}

void lexer_tknfile_processdirective(void)
{
    lexer_tknfile_incstackel_t *inctop;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    if(*curchar != '#')
        return;

    lexer_tknfile_advcurchar();
    if(!strncmp(curchar, "define", strlen("define")))
    {
        inctop->column += strlen("define");
        curchar += strlen("define");
        return lexer_tknfile_processdefine();
    }
    if(!strncmp(curchar, "include", strlen("include")))
    {
        inctop->column += strlen("include");
        curchar += strlen("include");
        return lexer_tknfile_processinclude();
    }
}

void lexer_tknfile_findnexttkn(void)
{
    int i;

    lexer_tknfile_incstackel_t *inctop;
    int len;
    char str[LEXER_MAXHARDTOKENLEN];
    int longestmatch, longestlen;
    char *tokenval;
    lexer_token_t newtkn;

    inctop = &((lexer_tknfile_incstackel_t*)lexer_tknfile_incstack.data)[lexer_tknfile_incstack.size - 1];

    if(!*curchar)
        return;

    if(*curchar == '#')
    {
        lexer_tknfile_processdirective();
        lexer_tknfile_skipwhitespace();
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
        fprintf(stderr, "\x1B[31merror in %s: \x1B[0minvalid token start %c at %lu:%lu.\n", inctop->filename, *curchar, inctop->line, inctop->column); 
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

    newtkn.line = inctop->line;
    newtkn.col = inctop->column;
    newtkn.file = inctop->filename;
    newtkn.type = longestmatch;
    newtkn.val = tokenval;

    list_push(&tokens, &newtkn);

    if(cli_verbose)
        printf("token \"%s\" (\"%s\") at %s:%lu:%lu.\n", tokenval, str, inctop->filename, inctop->line, inctop->column);
}

void lexer_tknfile_updatecomment(void)
{
    if(!curchar || !*curchar)
        return;

    if(incomment == LEXER_TKNFILE_COMMENTONELINER && *curchar == '\n')
    {
        incomment = 0;
        lexer_tknfile_advcurchar();
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
        lexer_tknfile_skipwhitespace();
    lexer_tknfile_updatecomment();
    if(!incomment)
        lexer_tknfile_skipwhitespace();

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

    lexer_tknfile_advcurchar();
    if(!incomment)
        stateprogress--;

    return true;
}

bool lexer_tknfile(srcfile_t* srcfile)
{
    int i;

    lexer_tknfile_incstackel_t srcstackel;

    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    state = LEXER_TOKENTYPE_INVALID;
    rawtext = curchar = strdup(srcfile->rawtext);
    incomment = 0;
    stateprogress = 0;
    list_initialize(&defines, sizeof(lexer_tknfile_define_t));
    list_initialize(&tokens, sizeof(lexer_token_t));
    list_initialize(&lexer_tknfile_incstack, sizeof(lexer_tknfile_incstackel_t));

    srcstackel.line = srcstackel.column = 1;
    strcpy(srcstackel.filename, srcfile->path);
    srcstackel.start = 0;
    srcstackel.end = strlen(rawtext);
    list_push(&lexer_tknfile_incstack, &srcstackel);

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

    list_free(&lexer_tknfile_incstack);

    free(rawtext);

    return true;
}
