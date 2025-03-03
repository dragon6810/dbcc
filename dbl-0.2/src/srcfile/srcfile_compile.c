#include <srcfile/srcfile.h>

#include <frontend/codegen/codegen.h>
#include <std/assert/assert.h>

bool srcfile_compile(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->rawtext);

    lexer_lexfile(srcfile);
    parser_parse(srcfile);
    codegen_gen(srcfile);

    return true;
}