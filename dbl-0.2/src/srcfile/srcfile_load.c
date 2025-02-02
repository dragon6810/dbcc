#include <srcfile/srcfile.h>

#include <stdio.h>
#include <string.h>

#include <assert/assert.h>

bool srcfile_load(char* path, srcfile_t* srcfile)
{
    FILE *ptr;
    unsigned long int len;

    assert(path);
    assert(srcfile);

    ptr = fopen(path, "r");
    if(!ptr)
        return false;

    fseek(ptr, 0, SEEK_END);
    len = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    srcfile->rawtext = malloc(len + 1);
    fread(srcfile->rawtext, 1, len, ptr);
    srcfile->rawtext[len] = 0;

    fclose(ptr);

    srcfile->path = strdup(path);

    return true;
}
