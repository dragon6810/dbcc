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

ir_instruction_t* codegen_gen_node(srcfile_t* srcfile, parser_astnode_t* node, ir_instruction_t* latestir);

ir_instruction_t* codegen_gen_compoundstatement(srcfile_t* srcfile, parser_astnode_t* node, bool fun, ir_instruction_t* latestir)
{
    ir_instruction_t* ir;

    assert(srcfile);
    assert(node);

    if(fun)
    {
        ir = calloc(1, sizeof(ir_instruction_t));
        ir->opcode = IR_INSTRUCTIONTYPE_RET;
        ir->params[0].value.type = IR_INSTRUCTION_PARAM_VALUE_TYPE_VOID;
        ir->params[0].value.flags |= IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGECONSTANT 
                                   & IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEBITS;

        if(latestir)
            latestir->next = ir;
        else
            latestir = srcfile->ir.instructions = ir;
        ir->last = latestir;

        latestir = ir;
    }

    return latestir;
}

ir_instruction_t* codegen_gen_functiondefinition(srcfile_t* srcfile, parser_astnode_t* node, ir_instruction_t* latestir)
{
    int i;

    assert(srcfile);
    assert(node);

    for(i=0; i<node->children.size; i++)
    {
        switch(node->children.data[i]->type)
        {
        case PARSER_NODETYPE_COMPOUNDSTATEMENT:
            latestir = codegen_gen_compoundstatement(srcfile, node->children.data[i], true, latestir);
        case PARSER_NODETYPE_DECLSPEC:
        case PARSER_NODETYPE_DECLARATOR:
            break;
        default:
            codegen_gen_panic(node->children.data[i], "expected declaration or function definition");
        }
    }

    return latestir;
}

ir_instruction_t* codegen_gen_externaldeclaration(srcfile_t* srcfile, parser_astnode_t* node, ir_instruction_t* latestir)
{
    int i;

    assert(srcfile);
    assert(node);

    for(i=0; i<node->children.size; i++)
    {
        switch(node->children.data[i]->type)
        {
        case PARSER_NODETYPE_DECL:
        case PARSER_NODETYPE_FUNCTIONDEF:
            latestir = codegen_gen_node(srcfile, node->children.data[i], latestir);

            break;
        default:
            codegen_gen_panic(node->children.data[i], "expected declaration or function definition");
        }
    }

    return latestir;
}

ir_instruction_t* codegen_gen_translationunit(srcfile_t* srcfile, parser_astnode_t* node, ir_instruction_t* latestir)
{
    int i;

    assert(srcfile);
    assert(node);
    
    for(i=0; i<node->children.size; i++)
    {
        codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_EXTERNALDECL);
        latestir = codegen_gen_node(srcfile, node->children.data[i], latestir);
    }

    return latestir;
}

ir_instruction_t* codegen_gen_node(srcfile_t* srcfile, parser_astnode_t* node, ir_instruction_t* latestir)
{
    char name[LEXER_MAXHARDTOKENLEN];

    assert(srcfile);
    assert(node);

    switch(node->type)
    {
    case PARSER_NODETYPE_TRANSLATIONUNIT:
        return codegen_gen_translationunit(srcfile, node, latestir);
        break;
    case PARSER_NODETYPE_EXTERNALDECL:
        return codegen_gen_externaldeclaration(srcfile, node, latestir);
        break;
    case PARSER_NODETYPE_FUNCTIONDEF:
        return codegen_gen_functiondefinition(srcfile, node, latestir);
        break;
    default:
        parser_typetostr(node->type, name);
        codegen_gen_panic(node, "Codegen nodetype TODO: \"%s\"", name);
        return latestir;
    }
}

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    codegen_gen_node(srcfile, srcfile->ast.nodes, srcfile->ir.instructions);

    if(cli_verbose)
        ir_print(&srcfile->ir);

    profiler_pop();
}