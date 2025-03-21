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

ir_declaration_variable_t codegen_gen_paramdeclaration(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    ir_declaration_variable_t decl;
    parser_astnode_t *typespec, *declarator, *directdecl;
    lexer_token_t *tkn;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_PARAMDECL);

    memset(&decl, 0, sizeof(decl));

    typespec = 0;
    for(i=0; i<node->children.size; i++)
    {
        if(node->children.data[i]->type != PARSER_NODETYPE_DECLSPEC)
            break;

        if(node->children.data[i]->children.data[0]->type != PARSER_NODETYPE_TYPESPEC)
            continue;

        typespec = node->children.data[i]->children.data[0];
    }

    if(!typespec)
    {
        tkn = codegen_gen_getfirsttokenofnode(node->children.data[i]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "expected type specifier");
    }

    if(typespec->children.data[0]->type != PARSER_NODETYPE_TERMINAL)
    {
        tkn = codegen_gen_getfirsttokenofnode(typespec->children.data[0]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    tkn = typespec->children.data[0]->token;

    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_INT:
        decl.type.type = IR_TYPE_I32;
        decl.type.name = strdup("i32");

        break;
    default:
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    codegen_gen_expectnodetype(node->children.data[node->children.size-1], PARSER_NODETYPE_DECLARATOR);
    declarator = node->children.data[node->children.size-1];

    codegen_gen_expectnodetype(declarator->children.data[declarator->children.size-1], PARSER_NODETYPE_DIRECTDECL);
    directdecl = declarator->children.data[declarator->children.size-1];

    codegen_gen_expectnodetype(directdecl->children.data[0], PARSER_NODETYPE_TERMINAL);
    codegen_gen_expecttoken(directdecl->children.data[0]->token, LEXER_TOKENTYPE_IDENTIFIER);
    decl.name = malloc(1 + strlen(directdecl->children.data[0]->token->val) + 1);
    strcpy(decl.name, "$");
    strcat(decl.name, directdecl->children.data[0]->token->val);

    return decl;
}

list_ir_declaration_variable_t codegen_gen_paramtypelist(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    list_ir_declaration_variable_t params;
    ir_declaration_variable_t decl;
    parser_astnode_t *paramlist;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_PARAMTYPELIST);

    codegen_gen_expectnodetype(node->children.data[0], PARSER_NODETYPE_PARAMLIST);
    paramlist = node->children.data[0];

    LIST_INITIALIZE(params);
    for(i=0; i<paramlist->children.size; i+=2) /* +2 to skip commas */
    {
        decl = codegen_gen_paramdeclaration(srcfile, paramlist->children.data[i]);
        LIST_PUSH(params, decl);
    }

    return params;
}

ir_declaration_function_t codegen_gen_functiondeclaration(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    parser_astnode_t *typespec, *initdecl, *declarator, *directdecl, *paramtypelist;
    lexer_token_t *tkn;
    ir_declaration_function_t decl;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_DECL);

    memset(&decl, 0, sizeof(decl));

    for(i=0, typespec=NULL; i<node->children.size; i++)
    {
        if(node->children.data[i]->type != PARSER_NODETYPE_DECLSPEC)
            break;

        if(node->children.data[i]->children.data[0]->type != PARSER_NODETYPE_TYPESPEC)
            continue;

        typespec = node->children.data[i]->children.data[0];
    }

    if(!typespec)
    {
        tkn = codegen_gen_getfirsttokenofnode(node->children.data[i]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "expected type specifier");
    }

    if(typespec->children.data[0]->type != PARSER_NODETYPE_TERMINAL)
    {
        tkn = codegen_gen_getfirsttokenofnode(typespec->children.data[0]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    tkn = typespec->children.data[0]->token;

    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_INT:
        decl.type.type = IR_TYPE_I32;
        decl.type.name = strdup("i32");

        break;
    default:
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_INITDECL);
    initdecl = node->children.data[i++];
    codegen_gen_expectnodetype(initdecl->children.data[0], PARSER_NODETYPE_DECLARATOR);
    declarator = initdecl->children.data[0];
    codegen_gen_expectnodetype(declarator->children.data[declarator->children.size-1], PARSER_NODETYPE_DIRECTDECL);
    directdecl = declarator->children.data[declarator->children.size-1];

    codegen_gen_expectnodetype(directdecl->children.data[0], PARSER_NODETYPE_TERMINAL);
    tkn = directdecl->children.data[0]->token;
    codegen_gen_expecttoken(tkn, LEXER_TOKENTYPE_IDENTIFIER);

    decl.name = malloc(1 + strlen(tkn->val) + 1);
    strcpy(decl.name, "@");
    strcat(decl.name, tkn->val);

    codegen_gen_expectnodetype(directdecl->children.data[1], PARSER_NODETYPE_TERMINAL);
    codegen_gen_expecttoken(directdecl->children.data[1]->token, LEXER_TOKENTYPE_OPENPARENTH);

    codegen_gen_expectnodetype(directdecl->children.data[2], PARSER_NODETYPE_PARAMTYPELIST);
    paramtypelist = directdecl->children.data[2];
    decl.parameters = codegen_gen_paramtypelist(srcfile, paramtypelist);
    if(paramtypelist->children.size>1)
        decl.variadic = true; /* more than just type list, should be ', ...' */

    codegen_gen_expectnodetype(directdecl->children.data[3], PARSER_NODETYPE_TERMINAL);
    codegen_gen_expecttoken(directdecl->children.data[3]->token, LEXER_TOKENTYPE_CLOSEPARENTH);

    return decl;
}

ir_declaration_t codegen_gen_declaration(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    ir_declaration_t decl;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_DECL);

    memset(&decl, 0, sizeof(decl));

    for(i=0; i<node->children.size; i++)
    {
        if(node->children.data[i]->type == PARSER_NODETYPE_INITDECL)
            break;
    }

    if(i >= node->children.size) /* what in tarnation */
        return decl;

    codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_INITDECL);
    codegen_gen_expectnodetype(node->children.data[i]->children.data[0], PARSER_NODETYPE_DECLARATOR);
    if(codegen_gen_isdeclaratorfuncdecl(srcfile, node->children.data[i]->children.data[0]))
    {
        decl.type = IR_DECLARATIONTYPE_FUNCTION;
        decl.function = codegen_gen_functiondeclaration(srcfile, node);
    }
    codegen_gen_expectnodetype(node->children.data[i+1], PARSER_NODETYPE_TERMINAL); /* ';' */

    return decl;
}

ir_definition_t codegen_gen_functiondef(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    parser_astnode_t *typespec, *declarator, *directdecl, *paramtypelist;
    ir_definition_t def;
    ir_definition_function_t *func;
    lexer_token_t *tkn;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_FUNCTIONDEF);

    memset(&def, 0, sizeof(def));

    def.type = IR_DECLARATIONTYPE_FUNCTION;
    func = &def.function;

    for(i=0, typespec=NULL; i<node->children.size; i++)
    {
        if(node->children.data[i]->type != PARSER_NODETYPE_DECLSPEC)
            break;

        if(node->children.data[i]->children.data[0]->type != PARSER_NODETYPE_TYPESPEC)
            continue;

        typespec = node->children.data[i]->children.data[0];
    }

    if(!typespec)
    {
        tkn = codegen_gen_getfirsttokenofnode(node->children.data[i]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "expected type specifier");
    }

    if(typespec->children.data[0]->type != PARSER_NODETYPE_TERMINAL)
    {
        tkn = codegen_gen_getfirsttokenofnode(typespec->children.data[0]);
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    tkn = typespec->children.data[0]->token;
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_INT:
        func->decl.type.type = IR_TYPE_I32;
        func->decl.type.name = strdup("i32");

        break;
    default:
        cli_errorsyntax(tkn->file, tkn->line, tkn->col, "unsupported type specifier");
    }

    codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_DECLARATOR);
    declarator = node->children.data[i];

    codegen_gen_expectnodetype(declarator->children.data[0], PARSER_NODETYPE_DIRECTDECL);
    directdecl = declarator->children.data[0];

    codegen_gen_expectnodetype(directdecl->children.data[0], PARSER_NODETYPE_TERMINAL);
    tkn = directdecl->children.data[0]->token;
    codegen_gen_expecttoken(tkn, LEXER_TOKENTYPE_IDENTIFIER);
    func->decl.name = malloc(1 + strlen(tkn->val) + 1);
    strcpy(func->decl.name, "@");
    strcat(func->decl.name, tkn->val);

    codegen_gen_expectnodetype(directdecl->children.data[2], PARSER_NODETYPE_PARAMTYPELIST);
    paramtypelist = directdecl->children.data[2];
    func->decl.parameters = codegen_gen_paramtypelist(srcfile, paramtypelist);
    if(paramtypelist->children.size > 1)
        func->decl.variadic = true;

    return def;
}

ir_declordef_t codegen_gen_externaldecl(srcfile_t* srcfile, parser_astnode_t* node)
{
    int i;

    ir_declordef_t declordef;
    ir_declaration_t *decl;
    ir_definition_t *def;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_EXTERNALDECL);

    memset(&declordef, 0, sizeof(declordef));

    for(i=0; i<node->children.size; i++)
    {
        switch(node->children.data[i]->type)
        {
        case PARSER_NODETYPE_DECL:
            decl = &declordef.decl;
            declordef.isdef = false;
            *decl = codegen_gen_declaration(srcfile, node->children.data[i]);

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
    char *name;
    ir_declaration_t *pdecl;
    ir_definition_t *pdef;

    assert(srcfile);
    assert(node);
    assert(node->type == PARSER_NODETYPE_TRANSLATIONUNIT);

    for(i=0; i<node->children.size; i++)
    {
        codegen_gen_expectnodetype(node->children.data[i], PARSER_NODETYPE_EXTERNALDECL);
        declordef = codegen_gen_externaldecl(srcfile, node->children.data[i]);

        if(declordef.isdef)
        {
            name = NULL;
            switch(declordef.decl.type)
            {
            case IR_DECLARATIONTYPE_FUNCTION:
                name = declordef.def.function.decl.name;
                break;
            case IR_DECLARATIONTYPE_VARIABLE:
                /*name = declordef.variable.decl.name;*/
                break;
            default:
                break;
            }

            /* has it been declared already? */
            if(!HASHMAP_FETCH(srcfile->ir.decls, name))
            {
                memset(&extradeclordef, 0, sizeof(extradeclordef));
                extradeclordef.isdef = false;
                extradeclordef.decl.type = IR_DECLARATIONTYPE_FUNCTION;
                extradeclordef.decl.function = declordef.def.function.decl;

                LIST_PUSH(srcfile->ir.body, extradeclordef);
                pdecl = &srcfile->ir.body.data[srcfile->ir.body.size-1].decl;
                HASHMAP_SET(srcfile->ir.decls, name, pdecl);
            }

            LIST_PUSH(srcfile->ir.body, declordef);
            pdef = &srcfile->ir.body.data[srcfile->ir.body.size-1].def;
            HASHMAP_SET(srcfile->ir.defs, name, pdef);
        }
        else
        {
            name = NULL;
            switch(declordef.decl.type)
            {
            case IR_DECLARATIONTYPE_FUNCTION:
                name = declordef.decl.function.name;
                break;
            case IR_DECLARATIONTYPE_VARIABLE:
                name = declordef.decl.variable.name;
                break;
            default:
                break;
            }

            LIST_PUSH(srcfile->ir.body, declordef);
            pdecl = &srcfile->ir.body.data[srcfile->ir.body.size-1].decl;
            HASHMAP_SET(srcfile->ir.decls, name, pdecl);
        }
    }
}

void codegen_gen(srcfile_t* srcfile)
{
    assert(srcfile);
    assert(srcfile->ast.nodes);

    profiler_push("Codegen");

    LIST_INITIALIZE(srcfile->ir.body);
    HASHMAP_INITIALIZE(srcfile->ir.decls, HASHMAP_BUCKETS_LARGE, string_ir_declaration_p);
    HASHMAP_INITIALIZE(srcfile->ir.defs, HASHMAP_BUCKETS_LARGE, string_ir_definition_p);

    codegen_gen_translationunit(srcfile, srcfile->ast.nodes);

    if(cli_verbose)
        ir_print(&srcfile->ir);

    profiler_pop();
}