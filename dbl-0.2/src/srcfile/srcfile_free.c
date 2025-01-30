#include <srcfile/srcfile.h>

#include <stdlib.h>

#include <assert/assert.h>

void srcfile_free(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    free(srcfile->path);
    free(srcfile->rawtext);
}