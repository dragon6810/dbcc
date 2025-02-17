#include <srcfile/srcfile.h>

#include <std/assert/assert.h>

bool srcfile_compile(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->rawtext);

    lexer_lexfile(srcfile);

    return true;
}