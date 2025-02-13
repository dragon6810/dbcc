#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>
#include <cli/cli.h>
#include <textutils/textutils.h>

void lexer_initialprocessing_cullcomments(lexer_state_t* state)
{
    int i, j, k;
    lexer_line_t *curline;

    lexer_statesrcel_t *stacktop;
    bool inblock;
    int startline, startcol;
    int endline, endcol;
    int linelen;
    lexer_line_t *pstartline, *pendline;
    char *str;

    stacktop = &LIST_FETCH(state->srcstack, lexer_statesrcel_t, state->srcstack.size - 1);

    for(i=0, inblock=false; i<stacktop->lines.size; i++)
    {
        linelen = strlen(curline->str);
        for(j=0; j<linelen; j++)
        {
            curline = &LIST_FETCH(stacktop->lines, lexer_line_t, i);

            if(!inblock && !strncmp(curline->str + j, "/*", 2))
            {
                inblock = true;
                startline = i;
                startcol = j;
                j++;
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
                    curline->str = textutils_insert(curline->str, " ", startcol);
                    linelen = strlen(curline->str);
                    continue;
                }

                printf("and here\n");

                pstartline = &stacktop->lines.data[startline];
                pendline = &stacktop->lines.data[endline];
                pstartline->str = textutils_remove(pstartline->str, startcol, strlen(pstartline->str));
                pendline->str = textutils_remove(pendline->str, 0, endcol);
                str = malloc(strlen(pstartline->str) + 1 + strlen(pendline->str) + 1);
                strcpy(str, pstartline->str);
                strcat(str, " ");
                strcat(str, pendline->str);
                free(pstartline->str);
                free(pendline->str);
                pstartline->str = str;
                puts(str);

                LIST_REMOVE(stacktop->lines, endline);

                for(k=startline+1; k<endline; k++)
                {
                    /* printf("removing line \"%s\".\n", stacktop->lines.data[startline + 1].str); */
                    LIST_REMOVE(stacktop->lines, startline + 1);
                }

                printf("and here..\n");
            }

            printf("over here too\n");
        }
    }
}

static void lexer_initialprocessing_mergelines(lexer_state_t* state)
{
    int i, j;
    lexer_line_t *curline;

    lexer_statesrcel_t *stacktop;
    char *newstr, *append;
    lexer_barrier_t barrier;

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
        strcat(newstr, append);

        barrier.line = curline->barriers.data[curline->barriers.size - 1].line + 1;
        barrier.column = append - (curline+1)->str + 1;
        barrier.position = strlen(curline->str);
        LIST_PUSH(curline->barriers, barrier);

        free(curline->str);
        free((curline+1)->str);
        curline->str = newstr;

        LIST_REMOVE(stacktop->lines, i + 1);
    }
}

void lexer_initialprocessing_splitlines(lexer_state_t* state)
{
    unsigned long int pos, i, j;
    lexer_line_t *curline;

    FILE *ptr;
    char *data;
    unsigned long int filelen;
    lexer_statesrcel_t *stacktop;
    lexer_line_t newline;
    lexer_barrier_t barrier;

    stacktop = &LIST_FETCH(state->srcstack, lexer_statesrcel_t, state->srcstack.size - 1);

    ptr = fopen(stacktop->filename, "r");
    if(!ptr)
        cli_errornofile("lexing", stacktop->filename);

    fseek(ptr, 0, SEEK_END);
    filelen = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    data = malloc(filelen + 1);
    fread(data, 1, filelen, ptr);

    for(pos=j=0, curline=NULL; pos<filelen;)
    {
        if(!curline || data[pos] == '\n')
        {   
            memset(&newline, 0, sizeof(newline));
            for(i=pos; (i<filelen) && (data[i]!='\n'); i++) {} i++; i -= pos;
            newline.str = calloc(i, 1);
            memcpy(newline.str, &data[pos], i - 1);
            pos += i;
            
            LIST_INITIALIZE(newline.barriers);
            barrier.line = j;
            barrier.column = barrier.position = 0;
            LIST_PUSH(newline.barriers, barrier);

            LIST_PUSH(stacktop->lines, newline);

            j++;
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