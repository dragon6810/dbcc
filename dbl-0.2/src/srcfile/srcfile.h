#ifndef _srcfile_h
#define _srcfile_h

#include <stdbool.h>

#include <frontend/lexer/lexer.h>
#include <std/list/list.h>

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct srcfile_s srcfile_t;

LIST_TYPE(srcfile_t, list_srcfile)

struct srcfile_s
{
    char *path, *rawtext;
    lexer_state_t lexer;
};

bool srcfile_load(char* path, srcfile_t* srcfile);
bool srcfile_compile(srcfile_t* srcfile);
void srcfile_free(srcfile_t* srcfile);

#endif