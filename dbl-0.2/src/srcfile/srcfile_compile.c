#include <srcfile/srcfile.h>

#include <std/assert/assert.h>

#include <backend/arch/arm/arm.h>
#include <backend/os/macho/macho.h>
#include <frontend/codegen/codegen.h>

bool srcfile_compile(srcfile_t* srcfile)
{
    // executable_t exec;

    assert(srcfile);
    assert(srcfile->rawtext);

    lexer_lexfile(srcfile);
    parser_parse(srcfile);
    codegen_gen(srcfile);
    arm_genasm(&srcfile->ir);
    //arm_makeexecutable(&srcfile->ir, &exec);
    //macho_writefile(&exec, "out");

    return true;
}