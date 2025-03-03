#include <frontend/codegen/codegen.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    ir_print(&srcfile->ir);

    profiler_pop();
}