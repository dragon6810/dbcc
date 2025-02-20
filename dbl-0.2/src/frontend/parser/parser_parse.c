#include <frontend/parser/parser.h>

#include <string.h>

#include <srcfile/srcfile.h>
#include <std/assert/assert.h>
#include <std/math/math.h>

void parser_parse_panicexpected(srcfile_t* srcfile, unsigned long int itoken, int n, ...)
{
    printf("parser: panic triggered\n");
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

lexer_token_t* parser_parse_peektoken(srcfile_t* srcfile, int n)
{
    return &srcfile->lexer.srcstack.data[srcfile->lexer.srcstack.size-1].tokens.data[srcfile->ast.curtok+n];
}

void parser_parse_consumedeclspecs(srcfile_t* srcfile, int parent)
{
    lexer_token_t* curtok;
    parser_astnode_t node, child;

    while(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER || 
          MATH_INRANGE(parser_parse_peektoken(srcfile, 0)->type, 
                       LEXER_TOKENTYPE_STARTOFKEYWORDS, LEXER_TOKENTYPE_ENDOFKEYWORDS))
    {
        if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER)
        {
            /* we're at the end of the declspecs */
            break;
        }

        curtok = parser_parse_consumetoken(srcfile);
        switch(curtok->type)
        {
        case LEXER_TOKENTYPE_AUTO:
        case LEXER_TOKENTYPE_REGISTER:
        case LEXER_TOKENTYPE_STATIC:
        case LEXER_TOKENTYPE_EXTERN:
        case LEXER_TOKENTYPE_TYPEDEF:

            /* storage class specifier */

            memset(&node, 0, sizeof(node));
            node.type = PARSER_NODETYPE_STORAGECLASSSPEC;
            LIST_INITIALIZE(node.children);
            LIST_RESIZE(node.children, 1);
            node.children.data[0] = srcfile->ast.nodes.size + 1;
            node.parent = parent;
            node.token = NULL;

            memset(&child, 0, sizeof(child));
            child.type = PARSER_NODETYPE_TERMINAL;
            child.parent = srcfile->ast.nodes.size;
            child.token = curtok;

            LIST_PUSH(srcfile->ast.nodes.data[parent].children, srcfile->ast.nodes.size);

            LIST_PUSH(srcfile->ast.nodes, node);
            LIST_PUSH(srcfile->ast.nodes, child);

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

            memset(&node, 0, sizeof(node));
            node.type = PARSER_NODETYPE_TYPESPEC;
            LIST_INITIALIZE(node.children);
            LIST_RESIZE(node.children, 1);
            node.children.data[0] = srcfile->ast.nodes.size + 1;
            node.parent = parent;
            node.token = NULL;

            memset(&child, 0, sizeof(child));
            child.type = PARSER_NODETYPE_TERMINAL;
            child.parent = srcfile->ast.nodes.size;
            child.token = curtok;

            LIST_PUSH(srcfile->ast.nodes.data[parent].children, srcfile->ast.nodes.size);

            LIST_PUSH(srcfile->ast.nodes, node);
            LIST_PUSH(srcfile->ast.nodes, child);

            break;
        case LEXER_TOKENTYPE_CONST:
        case LEXER_TOKENTYPE_VOLATILE:

            /* type qualifier */

            memset(&node, 0, sizeof(node));
            node.type = PARSER_NODETYPE_TYPEQUALIFIER;
            LIST_INITIALIZE(node.children);
            LIST_RESIZE(node.children, 1);
            node.children.data[0] = srcfile->ast.nodes.size + 1;
            node.parent = parent;
            node.token = NULL;

            memset(&child, 0, sizeof(child));
            child.type = PARSER_NODETYPE_TERMINAL;
            child.parent = srcfile->ast.nodes.size;
            child.token = curtok;

            LIST_PUSH(srcfile->ast.nodes.data[parent].children, srcfile->ast.nodes.size);

            LIST_PUSH(srcfile->ast.nodes, node);
            LIST_PUSH(srcfile->ast.nodes, child);

            break;
        }
    }
}

bool parser_parse_translationunit(srcfile_t* srcfile, bool panic);
bool parser_parse_externaldecl(srcfile_t* srcfile, int parent, bool panic);
bool parser_parse_functiondefinition(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);
bool parser_parse_declaration(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);

bool parser_parse_functiondefinition(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    return false;
}

/*
    <external-declaration> ::= <function-definition>
                             | <declaration>
*/
bool parser_parse_externaldecl(srcfile_t* srcfile, int parent, bool panic)
{
    parser_astnode_t newnode;
    int inewnode;

    newnode.type = PARSER_NODETYPE_EXTERNALDECL;
    LIST_INITIALIZE(newnode.children);
    newnode.parent = parent;
    newnode.token = NULL;

    inewnode = srcfile->ast.nodes.size;
    LIST_PUSH(srcfile->ast.nodes, newnode);
    LIST_PUSH(srcfile->ast.nodes.data[parent].children, inewnode);
    parser_parse_consumedeclspecs(srcfile, inewnode);

    return true;
}

/*
    <translation-unit> ::= {<external-declaration>}*
*/
bool parser_parse_translationunit(srcfile_t* srcfile, bool panic)
{
    parser_astnode_t root;

    memset(&root, 0, sizeof(root));
    root.type = PARSER_NODETYPE_TRANSLATIONUNIT;
    LIST_INITIALIZE(root.children);
    root.parent = -1;
    root.token = NULL;

    LIST_PUSH(srcfile->ast.nodes, root);

    srcfile->ast.curtok = 0;
    while(parser_parse_peektoken(srcfile, 0)->type != LEXER_TOKENTYPE_EOF)
    {
        if(!parser_parse_externaldecl(srcfile, 0, panic))
            return false;
        break;
    }

    return true;
}

void parser_parse(struct srcfile_s* srcfile)
{
    assert(srcfile);

    LIST_INITIALIZE(srcfile->ast.nodes);
    
    parser_parse_translationunit(srcfile, true);
    parser_print(&srcfile->ast);
}
