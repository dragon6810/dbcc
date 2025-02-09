#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <cli/cli.h>
#include <textutils/textutils.h>

static void lexer_initialprocessing_cullcomments(lexer_state_t* state)
{
    int i, j, k;
    lexer_line_t *curline;

    lexer_statesrcel_t *stacktop;
    bool inblock;
    int startline, startcol;
    int endline, endcol;
    int linelen;
    char *str;

    stacktop = &LIST_FETCH(state->srcstack, lexer_statesrcel_t, state->srcstack.size - 1);

    for(i=0, inblock=false; i<stacktop->lines.size; i++)
    {
        curline = &LIST_FETCH(stacktop->lines, lexer_line_t, i);
        linelen = strlen(curline->str);
        for(j=0; j<linelen; j++)
        {
            if(!inblock && !strncmp(curline->str + j, "/*", 2))
            {
                inblock = true;
                startline = i;
                startcol = j;
                j++;
                continue;
            }

            if(inblock && !strncmp(curline->str + j, "*/", 2))
            {
                inblock = false;
                endline = i;
                endcol = j + 2;
                j++;

                if(startline == endline)
                {
                    curline->str = textutils_remove(curline->str, startcol, endcol);
                    continue;
                }

                str = LIST_FETCH(stacktop->lines, lexer_line_t, startline).str;
                LIST_FETCH(stacktop->lines, lexer_line_t, startline).str = textutils_remove(str, startcol, strlen(str));
                str = LIST_FETCH(stacktop->lines, lexer_line_t, endline).str;
                LIST_FETCH(stacktop->lines, lexer_line_t, endline).str = textutils_remove(str, 0, endcol);

                for(k=startline+1; k<endline; k++)
                    list_remove(&stacktop->lines, startline + 1);
            }
        }
    }
}

static void lexer_initialprocessing_mergelines(lexer_state_t* state)
{
    int i, j;
    lexer_line_t *curline;

    lexer_statesrcel_t *stacktop;
    char *newstr, *append;
    unsigned long int newline;

    stacktop = &LIST_FETCH(state->srcstack, lexer_statesrcel_t, state->srcstack.size - 1);

    for(i=0; i<stacktop->lines.size; i++)
    {
        curline = &LIST_FETCH(stacktop->lines, lexer_line_t, i);
        
        for(j=strlen(curline->str)-1; (j>=0) && (curline->str[j]<=32); j--);
        if(j<0 || curline->str[j] != '\\' || i == stacktop->lines.size - 1)
            continue;

        /* line terminated with backslash, so merge it with the next line */

        append = (curline+1)->str;
        while(*append && *append <= 32)
            append++;
        
        newstr = malloc(strlen(curline->str) - 1 + strlen(append) + 1);
        strcpy(newstr, curline->str);
        newstr[j] = 0;
        newline = j;
        list_push(&curline->newlines, &newline);
        strcat(newstr, append);
        free(curline->str);
        curline->str = newstr;

        free((curline+1)->str);
        list_remove(&stacktop->lines, i + 1);
        i--;
    }
}

static void lexer_initialprocessing_splitlines(lexer_state_t* state)
{
    unsigned long int pos, i;
    lexer_line_t *curline;

    FILE *ptr;
    char *data;
    unsigned long int filelen;
    lexer_statesrcel_t *stacktop;
    lexer_line_t newline;

    stacktop = &LIST_FETCH(state->srcstack, lexer_statesrcel_t, state->srcstack.size - 1);

    ptr = fopen(stacktop->filename, "r");
    if(!ptr)
        cli_errornofile("lexing", stacktop->filename);

    fseek(ptr, 0, SEEK_END);
    filelen = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    data = malloc(filelen + 1);
    fread(data, 1, filelen, ptr);

    for(pos=0, curline=NULL; pos<filelen;)
    {
        if(!curline || data[pos] == '\n')
        {   
            memset(&newline, 0, sizeof(newline));
            for(i=pos; (i<filelen) && (data[i]!='\n'); i++) {} i++; i -= pos;
            newline.str = calloc(i, 1);
            memcpy(newline.str, &data[pos], i - 1);
            pos += i;
            
            list_initialize(&newline.newlines, sizeof(unsigned long int));
            list_push(&newline.newlines, &i);

            list_push(&stacktop->lines, &newline);
        }
    }
    
    free(data);
    fclose(ptr);
}

bool lexer_initialprocessing(lexer_state_t* state)
{
    assert(state);

    lexer_initialprocessing_splitlines(state);
    lexer_initialprocessing_mergelines(state);
    lexer_initialprocessing_cullcomments(state);

    return true;
}