#include <frontend/codegen/codegen.h>

#include <cli/cli.h>
#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

lexer_token_t* codegen_gen_getfirsttokenofnode(parser_astnode_t* node)
{
    assert(node);

    if(node->type == PARSER_NODETYPE_TERMINAL)
        return node->token;

    assert(node->children.size);

    return codegen_gen_getfirsttokenofnode(node->children.data[0]);
}

void codegen_gen_panic(parser_astnode_t* node, const char* format, ...)
{
    va_list args;
    lexer_token_t *tkn;

    tkn = codegen_gen_getfirsttokenofnode(node);

    va_start(args, format);
    cli_verrorsyntax(tkn->file, tkn->line, tkn->col, format, args);
    va_end(args);

    abort();
}

void codegen_gen_expectnodetype(parser_astnode_t* node, parser_nodetype_e type)
{
    char name[LEXER_MAXHARDTOKENLEN];

    assert(node);

    if(node->type == type)
        return;

    parser_typetostr(node->type, name);
    codegen_gen_panic(node, "expected %s", name);

    abort();
}

void codegen_gen_translationunit(srcfile_t* srcfile, parser_astnode_t* node)
{
    puts("gen");
}

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    codegen_gen_translationunit(srcfile, srcfile->ast.nodes);

    if(cli_verbose)
        ir_print(&srcfile->ir);

    profiler_pop();
}