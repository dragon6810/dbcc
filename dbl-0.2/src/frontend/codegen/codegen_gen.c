#include <frontend/codegen/codegen.h>

#include <cli/cli.h>
#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

void codegen_gen_node(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    assert(srcfile);
    assert(node);

    for(i=0; i<node->children.size; i++)
        codegen_gen_node(srcfile, node->children.data[i]);
}

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    codegen_gen_node(srcfile, srcfile->ast.nodes);

    if(cli_verbose)
        ir_print(&srcfile->ir);

    profiler_pop();
}