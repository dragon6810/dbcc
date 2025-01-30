#ifndef _srcfile_h
#define _srcfile_h

#include <stdbool.h>

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct srcfile_s srcfile_t;

struct srcfile_s
{
    char *path, *rawtext;
};

bool srcfile_load(char* path, srcfile_t* srcfile);
void srcfile_free(srcfile_t* srcfile);

#endif