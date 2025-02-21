#include <frontend/parser/parser.h>

#include <stdarg.h>
#include <string.h>

#include <cli/cli.h>
#include <srcfile/srcfile.h>
#include <std/assert/assert.h>
#include <std/math/math.h>

void parser_parse_panic(srcfile_t* srcfile, lexer_token_t* tkn, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    cli_verrorsyntax(tkn->file, tkn->line, tkn->col, format, args);

    va_end(args);
    abort();
}

int parser_parse_istokendeclspec(lexer_token_t* tkn)
{
    return 0;
}

lexer_token_t* parser_parse_consumetoken(srcfile_t* srcfile)
{
    return &srcfile->lexer.srcstack.data[srcfile->lexer.srcstack.size-1].tokens.data[srcfile->ast.curtok++];
}

lexer_token_t* parse_parse_jumprelativetoken(srcfile_t* srcfile, int jump)
{
    return &srcfile->lexer.srcstack.data[srcfile->lexer.srcstack.size-1].tokens.data[srcfile->ast.curtok+=jump];
}

lexer_token_t* parser_parse_expecttoken(srcfile_t* srcfile, lexer_tokentype_e type)
{
    lexer_token_t *tkn;
    char name[LEXER_MAXHARDTOKENLEN];

    tkn = parser_parse_consumetoken(srcfile);
    if(tkn->type != type)
    {
        lexer_tkntypetostring(type, name);
        parser_parse_panic(srcfile, tkn, "expected %s", name);
    }

    return tkn;
}

lexer_token_t* parser_parse_peektoken(srcfile_t* srcfile, int n)
{
    return &srcfile->lexer.srcstack.data[srcfile->lexer.srcstack.size-1].tokens.data[srcfile->ast.curtok+n];
}

list_parser_astnode_p_t parser_parse_consumedeclspecs(srcfile_t* srcfile, parser_astnode_t* parent)
{
    lexer_token_t* curtok;
    parser_astnode_t *node, *child;
    list_parser_astnode_p_t nodes;

    LIST_INITIALIZE(nodes);

    while(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER || 
          MATH_INRANGE(parser_parse_peektoken(srcfile, 0)->type, 
                       LEXER_TOKENTYPE_STARTOFKEYWORDS, LEXER_TOKENTYPE_ENDOFKEYWORDS))
    {
        if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER)
        {
            /* we're at the end of the declspecs */
            break;
        }

        node = malloc(sizeof(parser_astnode_t));
        child = malloc(sizeof(parser_astnode_t));
        memset(node, 0, sizeof(parser_astnode_t));
        memset(child, 0, sizeof(parser_astnode_t));

        curtok = parser_parse_consumetoken(srcfile);
        switch(curtok->type)
        {
        case LEXER_TOKENTYPE_AUTO:
        case LEXER_TOKENTYPE_REGISTER:
        case LEXER_TOKENTYPE_STATIC:
        case LEXER_TOKENTYPE_EXTERN:
        case LEXER_TOKENTYPE_TYPEDEF:

            /* storage class specifier */

            node->type = PARSER_NODETYPE_STORAGECLASSSPEC;

            break;
        case LEXER_TOKENTYPE_VOID:
        case LEXER_TOKENTYPE_CHAR:
        case LEXER_TOKENTYPE_SHORT:
        case LEXER_TOKENTYPE_INT:
        case LEXER_TOKENTYPE_LONG:
        case LEXER_TOKENTYPE_FLOAT:
        case LEXER_TOKENTYPE_DOUBLE:
        case LEXER_TOKENTYPE_SIGNED:
        case LEXER_TOKENTYPE_UNSIGNED:
        /* TODO: type names for structs, unions, enums, and typedefs */

            /* type specifier */

            node->type = PARSER_NODETYPE_TYPESPEC;

            break;
        case LEXER_TOKENTYPE_CONST:
        case LEXER_TOKENTYPE_VOLATILE:

            /* type qualifier */

            node->type = PARSER_NODETYPE_TYPEQUALIFIER;

            break;
        }
    
        LIST_INITIALIZE(node->children);
        LIST_RESIZE(node->children, 1);
        node->children.data[0] = child;
        node->parent = parent;
        node->token = NULL;
        
        LIST_INITIALIZE(child->children);
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = curtok;

        LIST_PUSH(nodes, node);
    }

    return nodes;
}

parser_astnode_t* parser_parse_allocnode(void)
{
    parser_astnode_t *node;

    node = malloc(sizeof(parser_astnode_t));
    memset(node, 0, sizeof(parser_astnode_t));

    return node;
}

parser_astnode_t* parser_parse_declarator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);

/*
    <parameter-declaration> ::= {<declaration-specifier>}+ <declarator>
                              | {<declaration-specifier>}+
*/
parser_astnode_t* parser_parse_parameterdeclaration(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    int i;

    parser_astnode_t *node, *child;
    list_parser_astnode_p_t declspecs;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_PARAMDECL;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    declspecs = parser_parse_consumedeclspecs(srcfile, node);
    if(!declspecs.size)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "expected declaration specifier");

    for(i=0; i<declspecs.size; i++)
        LIST_PUSH(node->children, declspecs.data[i]);

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_ASTERISK || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER
    )
    {
        /* declarator */

        child = parser_parse_declarator(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <parameter-list> ::= <parameter-declaration>
                       | <parameter-list> , <parameter-declaration>
*/
parser_astnode_t* parser_parse_parameterlist(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_PARAMLIST;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_parameterdeclaration(srcfile, node, panic);
    LIST_PUSH(node->children, child);
    while(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_COMMA)
    {
        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_COMMA);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_parameterdeclaration(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <parameter-type-list> ::= <parameter-list>
                            | <parameter-list> , ...
*/
parser_astnode_t* parser_parse_parametertypelist(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_PARAMTYPELIST;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_parameterlist(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_COMMA)
    {
        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_COMMA);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_COMMA);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <direct-declarator> ::= <identifier>
                          | ( <declarator> )
                          | <direct-declarator> [ {<constant-expression>}? ]
                          | <direct-declarator> ( <parameter-type-list> )
*/
parser_astnode_t* parser_parse_directdeclarator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child, *term;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_DIRECTDECL;
    LIST_INITIALIZE(node->children);
    node->parent = parent;
    
    tkn = parser_parse_consumetoken(srcfile);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_OPENPARENTH:
        term = parser_parse_allocnode();
        term->type = PARSER_NODETYPE_TERMINAL;
        term->parent = node;
        term->token = tkn;
        LIST_PUSH(node->children, term);

        child = parser_parse_declarator(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_CLOSEPARENTH);
        term = parser_parse_allocnode();
        term->type = PARSER_NODETYPE_TERMINAL;
        term->parent = node;
        term->token = tkn;
        LIST_PUSH(node->children, term);

        break;
    case LEXER_TOKENTYPE_IDENTIFIER:
        term = parser_parse_allocnode();
        term->type = PARSER_NODETYPE_TERMINAL;
        term->parent = node;
        term->token = tkn;
        LIST_PUSH(node->children, term);

        break;
    default:
        parser_parse_panic(srcfile, tkn, "expected '(' or identifier");
    }

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OPENPARENTH || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OPENBRACKET
    )
    {
        switch((tkn = parser_parse_consumetoken(srcfile))->type)
        {
        case LEXER_TOKENTYPE_OPENPARENTH:
            term = parser_parse_allocnode();
            term->type = PARSER_NODETYPE_TERMINAL;
            term->parent = node;
            term->token = tkn;
            LIST_PUSH(node->children, term);

            child = parser_parse_parametertypelist(srcfile, node, panic);
            LIST_PUSH(node->children, child);

            tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_CLOSEPARENTH);
            term = parser_parse_allocnode();
            term->type = PARSER_NODETYPE_TERMINAL;
            term->parent = node;
            term->token = tkn;
            LIST_PUSH(node->children, term);
            
            break;
        case LEXER_TOKENTYPE_OPENBRACKET:

            parser_parse_panic(srcfile, tkn, "TODO: <direct-declarator> [ {<constant-expression>}? ]");

            break;
        default:
            /* wtf */
            break;
        }
    }

    return node;
}

/*
    <pointer> ::= * {<type-qualifier>}* {<pointer>}?
*/
parser_astnode_t* parser_parse_pointer(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *asterisk, *typequal, *typequalterm, *pointer;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_POINTER;
    LIST_INITIALIZE(node->children);
    node->parent = parent;
    node->token = NULL;

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_ASTERISK);
    asterisk = parser_parse_allocnode();
    asterisk->type = PARSER_NODETYPE_TERMINAL;
    asterisk->parent = node;
    asterisk->token = tkn;
    LIST_PUSH(node->children, asterisk);

    while(true)
    {
        tkn = parser_parse_peektoken(srcfile, 0);
        if(tkn->type != LEXER_TOKENTYPE_CONST && tkn->type != LEXER_TOKENTYPE_VOLATILE)
            break;

        typequal = parser_parse_allocnode();
        typequal->type = PARSER_NODETYPE_TYPEQUALIFIER;
        LIST_INITIALIZE(typequal->children);
        typequal->parent = node;
        LIST_PUSH(node->children, asterisk);

        typequalterm = parser_parse_allocnode();
        typequalterm->type = PARSER_NODETYPE_TERMINAL;
        typequalterm->parent = typequal;
        typequalterm->token = tkn;
        LIST_PUSH(typequal->children, typequalterm);

        parser_parse_consumetoken(srcfile);
    }

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_ASTERISK)
    {
        pointer = parser_parse_pointer(srcfile, node, panic);
        LIST_PUSH(node->children, pointer);
    }

    return node;
}

/*
    <declarator> ::= {<pointer>}? <direct-declarator>
*/
parser_astnode_t* parser_parse_declarator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *pointer, *directdecl;

    node = pointer = directdecl = NULL;

    node = malloc(sizeof(parser_astnode_t));
    memset(node, 0, sizeof(parser_astnode_t));

    node->type = PARSER_NODETYPE_DECLARATOR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;
    node->token = NULL;

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_ASTERISK)
    {
        pointer = parser_parse_pointer(srcfile, node, panic);
        LIST_PUSH(node->children, pointer);
    }

    directdecl = parser_parse_directdeclarator(srcfile, node, panic);
    if(!directdecl)
        goto cleanupfail;
    LIST_PUSH(node->children, directdecl);

    return node;

cleanupfail:
    if(node)
        parser_freenode(node);

    parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "expected direct declaration");
    return NULL;
}

parser_astnode_t* parser_parse_declaration(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    return NULL;
}

/*
    <function-definition> ::= {<declaration-specifier>}* <declarator> <compound-statement>
                                                                      ^ we start here in this function
*/
parser_astnode_t* parser_parse_functiondefinition(srcfile_t* srcfile, parser_astnode_t* parent, list_parser_astnode_p_t declspecs, parser_astnode_t* declarator, bool panic)
{
    int i;

    parser_astnode_t* node;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_FUNCTIONDEF;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    for(i=0; i<declspecs.size; i++)
    {
        declspecs.data[i]->parent = node;
        LIST_PUSH(node->children, declspecs.data[i]);
    }
    LIST_PUSH(node->children, declarator);

    return node;
}

/*
    <external-declaration> ::= <function-definition>
                             | <declaration>
*/
parser_astnode_t* parser_parse_externaldecl(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *newnode, *declarator, *child;
    list_parser_astnode_p_t declspecs;

    newnode = declarator = 0;

    newnode = malloc(sizeof(parser_astnode_t));
    newnode->type = PARSER_NODETYPE_EXTERNALDECL;
    LIST_INITIALIZE(newnode->children);
    newnode->parent = parent;
    newnode->token = NULL;

    declspecs = parser_parse_consumedeclspecs(srcfile, newnode);
    declarator = parser_parse_declarator(srcfile, parent, panic);

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_ASSIGN    || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_COMMA     ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_SEMICOLON
    )
    {
        /* declaration */

        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "TODO: <external declaration> -> <declaration>");
    }
    else
    {
        /* function definition */
        child = parser_parse_functiondefinition(srcfile, newnode, declspecs, declarator, panic);
        LIST_PUSH(newnode->children, child);
    }

    return newnode;
}

/*
    <translation-unit> ::= {<external-declaration>}*
*/
parser_astnode_t* parser_parse_translationunit(srcfile_t* srcfile, bool panic)
{
    parser_astnode_t *root, *child;

    root = parser_parse_allocnode();
    root->type = PARSER_NODETYPE_TRANSLATIONUNIT;
    LIST_INITIALIZE(root->children);
    root->parent = NULL;
    root->token = NULL;

    srcfile->ast.nodes = root;

    srcfile->ast.curtok = 0;
    while(parser_parse_peektoken(srcfile, 0)->type != LEXER_TOKENTYPE_EOF)
    {
        if(!(child = parser_parse_externaldecl(srcfile, 0, panic)))
            goto fail;

        LIST_PUSH(root->children, child);
        break;
    }

    return root;

fail:
    if(root)
        free(root);

    return NULL;
}

void parser_parse(struct srcfile_s* srcfile)
{
    assert(srcfile);
    
    srcfile->ast.nodes = parser_parse_translationunit(srcfile, true);
    parser_print(&srcfile->ast);
}
