#include <frontend/codegen/codegen.h>

#include <string.h>

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

    parser_typetostr(type, name);
    codegen_gen_panic(node, "expected %s", name);

    abort();
}

lexer_token_t* codegen_gen_expecttoken(lexer_token_t* tkn, lexer_tokentype_e type)
{
    char name[LEXER_MAXHARDTOKENLEN];

    if(tkn->type != type)
    {
        lexer_tkntypetostring(type, name);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "expected %s", name);
    }

    return tkn;
}

bool codegen_gen_isdeclaratorfuncdecl(srcfile_t* srcfile, parser_astnode_t* declarator)
{
    parser_astnode_t *direct;

    assert(srcfile);
    assert(declarator);
    assert(declarator->type == PARSER_NODETYPE_DECLARATOR);

    direct = declarator->children.data[declarator->children.size-1];
    codegen_gen_expectnodetype(direct, PARSER_NODETYPE_DIRECTDECL);

    if(direct->children.size < 2)
        return false;

    if(direct->children.data[1]->type != PARSER_NODETYPE_TERMINAL)
        return false;

    if(direct->children.data[1]->token->type != LEXER_TOKENTYPE_OPENPARENTH)
        return false;

    return true;
}

ir_regindex_t codegen_gen_expression(srcfile_t* srcfile, ir_definition_function_t* func, parser_astnode_t* exp)
{
    ir_instruction_t* in;
    ir_instruction_t* last;

    assert(srcfile);
    assert(func);
    assert(exp);
    assert(exp->type == PARSER_NODETYPE_EXPR);

    // TODO: actually parse expression

    in = calloc(1, sizeof(ir_instruction_t));
    in->opcode = IR_INSTRUCTIONTYPE_LOADCONST;

    in->loadconst.reg.reg = func->nregisters++;
    in->loadconst.val.val = 0;

    last = func->instructions;
    while(last && last->next)
        last = last->next;
    if(last)
    {
        last->next = in;
        in->last = last;
    }
    else
    {
        func->instructions = in;
    }

    return func->nregisters - 1;
}

ir_instruction_t* codegen_gen_returnstatement(srcfile_t* srcfile, ir_definition_function_t* func, parser_astnode_t* statement)
{
    ir_instruction_t *in;

    assert(srcfile);
    assert(statement);
    assert(statement->type == PARSER_NODETYPE_JUMPSTATEMENT);
    assert(statement->children.size == 3); // return expression ;

    in = calloc(1, sizeof(ir_instruction_t));
    in->opcode = IR_INSTRUCTIONTYPE_RETURN;
    in->ret.reg.reg = codegen_gen_expression(srcfile, func, statement->children.data[1]);

    return in;
}

ir_instruction_t* codegen_gen_jumpstatement(srcfile_t* srcfile, ir_definition_function_t* func, parser_astnode_t* statement)
{
    parser_astnode_t *child;
    ir_instruction_t *in;
    lexer_token_t *tkn;

    assert(srcfile);
    assert(statement);
    assert(statement->type == PARSER_NODETYPE_JUMPSTATEMENT);

    codegen_gen_expectnodetype(statement->children.data[0], PARSER_NODETYPE_TERMINAL);
    child = statement->children.data[0];
    tkn = child->token;

    in = NULL;
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_GOTO:
    case LEXER_TOKENTYPE_CONTINUE:
    case LEXER_TOKENTYPE_BREAK:
        codegen_gen_panic(child, "CODEGEN: TODO: unsupported jump statement");
        break;
    case LEXER_TOKENTYPE_RETURN:
        in = codegen_gen_returnstatement(srcfile, func, statement);
        break;
    default:
        codegen_gen_panic(child, "expected jump statement");
        break;
    }

    return in;
}

ir_instruction_t* codegen_gen_statement(srcfile_t* srcfile, ir_definition_function_t* func, parser_astnode_t* statement)
{
    parser_astnode_t *child;
    ir_instruction_t *first;

    assert(srcfile);
    assert(statement);
    assert(statement->type == PARSER_NODETYPE_STATEMENT);

    first = NULL;
    child = statement->children.data[0];
    switch(child->type)
    {
    case PARSER_NODETYPE_JUMPSTATEMENT:
        first = codegen_gen_jumpstatement(srcfile, func, child);
        break;
    default:
        codegen_gen_panic(child, "CODEGEN: TODO: unsupported statement type");
    }

    return first;
}

void codegen_gen_functionbody(srcfile_t* srcfile, ir_definition_function_t* func, parser_astnode_t* funcdef)
{
    int i;

    parser_astnode_t *body, *cur;
    ir_instruction_t *last, *in;

    assert(srcfile);
    assert(funcdef);
    assert(funcdef->type == PARSER_NODETYPE_FUNCTIONDEF);

    codegen_gen_expectnodetype(funcdef->children.data[funcdef->children.size-1], PARSER_NODETYPE_COMPOUNDSTATEMENT);
    body = funcdef->children.data[funcdef->children.size-1];

    last = NULL;
    for(i=1; i<body->children.size-1; i++) /* avoid curly braces */
    {
        cur = body->children.data[i];
        switch(cur->type)
        {
        case PARSER_NODETYPE_DECL:
            codegen_gen_panic(cur, "CODEGEN: TODO: Declaration");
            break;
        case PARSER_NODETYPE_STATEMENT:
            in = codegen_gen_statement(srcfile, func, cur);
            if(!last)
                last = func->instructions;
            while(last && last->next)
                last = last->next;
            if(last)
            {
                last->next = in;
                in->last = last;
            }
            else
            {
                func->instructions = in;
            }
            last = in;
            break;
        default:
            break;
        }
    }
}

ir_definition_function_t codegen_gen_functiondef(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    parser_astnode_t *typespec, *declarator, *directdecl;
    ir_definition_function_t def;
    lexer_token_t *tkn;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_FUNCTIONDEF);

    memset(&def, 0, sizeof(def));

    for(i=0, typespec=NULL; i<node->children.size; i++)
    {
        if(node->children.data[i]->type != PARSER_NODETYPE_DECLSPEC)
            break;
    }

    if(i >= node->children.size)
    {
        codegen_gen_panic(node, "expected declarator\n");
    }

    codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_DECLARATOR);
    declarator = node->children.data[i];

    codegen_gen_expectnodetype(declarator->children.data[0], PARSER_NODETYPE_DIRECTDECL);
    directdecl = declarator->children.data[0];

    codegen_gen_expectnodetype(directdecl->children.data[0], PARSER_NODETYPE_TERMINAL);
    tkn = directdecl->children.data[0]->token;
    codegen_gen_expecttoken(tkn, LEXER_TOKENTYPE_IDENTIFIER);
    def.decl.name = malloc(1 + strlen(tkn->val) + 1);
    strcpy(def.decl.name, "@");
    strcat(def.decl.name, tkn->val);

    codegen_gen_functionbody(srcfile, &def, node);

    return def;
}

ir_declordef_t codegen_gen_externaldecl(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    ir_declordef_t declordef;
    ir_definition_function_t *def;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_EXTERNALDECL);

    memset(&declordef, 0, sizeof(declordef));

    for(i=0; i<node->children.size; i++)
    {
        switch(node->children.data[i]->type)
        {
        case PARSER_NODETYPE_DECL:
            codegen_gen_panic(node, "CODEGEN: TODO: declarations");

            break;
        case PARSER_NODETYPE_FUNCTIONDEF:
            def = &declordef.def;
            declordef.isdef = true;
            *def = codegen_gen_functiondef(srcfile, node->children.data[i]);

            break;
        default:
            break;
        }
    }

    return declordef;
}

void codegen_gen_translationunit(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    ir_declordef_t declordef, extradeclordef;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_TRANSLATIONUNIT);

    for(i=0; i<node->children.size; i++)
    {
        codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_EXTERNALDECL);
        declordef = codegen_gen_externaldecl(srcfile, node->children.data[i]);

        if(declordef.isdef)
        {
            /* if it hasn't been declared already, make a declaration */
            memset(&extradeclordef, 0, sizeof(extradeclordef));
            extradeclordef.isdef = false;
            extradeclordef.decl = declordef.def.decl;

            LIST_PUSH(srcfile->ir.body, extradeclordef);

            LIST_PUSH(srcfile->ir.body, declordef);
        }
        else
        {
            codegen_gen_panic(node->children.data[i], "CODEGEN: TODO: support declarations\n");
        }
    }
}

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    LIST_INITIALIZE(srcfile->ir.body);

    codegen_gen_translationunit(srcfile, srcfile->ast.nodes);

    if(cli_verbose)
        ir_print(&srcfile->ir);

    profiler_pop();
}