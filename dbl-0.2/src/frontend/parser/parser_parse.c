#include <frontend/parser/parser.h>

#include <stdarg.h>
#include <string.h>

#include <cli/cli.h>
#include <srcfile/srcfile.h>
#include <std/assert/assert.h>
#include <std/math/math.h>
#include <std/profiler/profiler.h>

void parser_parse_panic(srcfile_t* srcfile, lexer_token_t* tkn, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    cli_verrorsyntax(tkn->file, tkn->line, tkn->col, format, args);

    va_end(args);
    abort();
}

bool parser_parse_istokendeclspec(lexer_token_t* tkn)
{
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_AUTO:
    case LEXER_TOKENTYPE_REGISTER:
    case LEXER_TOKENTYPE_STATIC:
    case LEXER_TOKENTYPE_EXTERN:
    case LEXER_TOKENTYPE_TYPEDEF:
    case LEXER_TOKENTYPE_VOID:
    case LEXER_TOKENTYPE_CHAR:
    case LEXER_TOKENTYPE_SHORT:
    case LEXER_TOKENTYPE_INT:
    case LEXER_TOKENTYPE_LONG:
    case LEXER_TOKENTYPE_FLOAT:
    case LEXER_TOKENTYPE_DOUBLE:
    case LEXER_TOKENTYPE_SIGNED:
    case LEXER_TOKENTYPE_UNSIGNED:
    case LEXER_TOKENTYPE_CONST:
    case LEXER_TOKENTYPE_VOLATILE:
        return true;
    default:
        return false;
    }
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

char* parser_parse_getdeclaratorname(parser_astnode_t* node)
{
    parser_astnode_t *directdecl;

    assert(node);
    assert(node->type = PARSER_NODETYPE_DECLARATOR);

    directdecl = node->children.data[node->children.size-1];
    assert(directdecl->children.data[0]->type == PARSER_NODETYPE_TERMINAL);

    return directdecl->children.data[0]->token->val;
}

parser_astnode_t* parser_parse_allocnode(void)
{
    parser_astnode_t *node;

    node = malloc(sizeof(parser_astnode_t));
    memset(node, 0, sizeof(parser_astnode_t));

    return node;
}

parser_astnode_t* parser_parse_expression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);
parser_astnode_t* parser_parse_declarator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);
parser_astnode_t* parser_parse_compoundstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);
parser_astnode_t* parser_parse_castexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic);

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

/*
    <primary-expression> ::= <identifier>
                           | <constant>
                           | <string>
                           | ( <expression> )
*/
parser_astnode_t* parser_parse_primaryexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_PRIMARYEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    switch((tkn = parser_parse_consumetoken(srcfile))->type)
    {
    case LEXER_TOKENTYPE_IDENTIFIER:
    case LEXER_TOKENTYPE_CONSTANT:
    case LEXER_TOKENTYPE_CHARCONSTANT:
    case LEXER_TOKENTYPE_STRING:
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);
        
        break;
    case LEXER_TOKENTYPE_OPENPARENTH:
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_expression(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_CLOSEPARENTH);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        parser_parse_panic(srcfile, tkn, "expected expression");

        break;
    }
    

    return node;
}

/*
    <postfix-expression> ::= <primary-expression>
                           | <postfix-expression> [ <expression> ]
                           | <postfix-expression> ( {<assignment-expression>}* )
                           | <postfix-expression> . <identifier>
                           | <postfix-expression> -> <identifier>
                           | <postfix-expression> ++
                           | <postfix-expression> --
*/
parser_astnode_t* parser_parse_postfixexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_POSTFIXEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_primaryexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    tkn = parser_parse_peektoken(srcfile, 0);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_INCREMENT:
    case LEXER_TOKENTYPE_DECREMENT:
        tkn = parser_parse_consumetoken(srcfile);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        break;
    }

    return node;
}

/*
    <unary-operator> ::= &
                       | *
                       | +
                       | -
                       | ~
                       | !
*/
parser_astnode_t* parser_parse_unaryoperator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_UNARYOP;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_consumetoken(srcfile);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_AMPERSAND:
    case LEXER_TOKENTYPE_ASTERISK:
    case LEXER_TOKENTYPE_PLUS:
    case LEXER_TOKENTYPE_MINUS:
    case LEXER_TOKENTYPE_BITNOT:
    case LEXER_TOKENTYPE_NOT:
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        parser_parse_panic(srcfile, tkn, "expected unary operator");
    }

    return node;
}

/*
    <unary-expression> ::= <postfix-expression>
                         | ++ <unary-expression>
                         | -- <unary-expression>
                         | <unary-operator> <cast-expression>
                         | sizeof <unary-expression>
                         | sizeof <type-name>
*/
parser_astnode_t* parser_parse_unaryexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_UNARYEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_peektoken(srcfile, 0);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_INCREMENT:
    case LEXER_TOKENTYPE_DECREMENT:
        parser_parse_consumetoken(srcfile);

        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_unaryexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        return node;
    case LEXER_TOKENTYPE_AMPERSAND:
    case LEXER_TOKENTYPE_ASTERISK:
    case LEXER_TOKENTYPE_PLUS:
    case LEXER_TOKENTYPE_MINUS:
    case LEXER_TOKENTYPE_BITNOT:
    case LEXER_TOKENTYPE_NOT:
        child = parser_parse_unaryoperator(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        child = parser_parse_castexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        return node;
    default:
        child = parser_parse_postfixexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        break;
    }

    return node;
}

/*
    <cast-expression> ::= <unary-expression>
                        | ( <type-name> ) <cast-expression>
*/
parser_astnode_t* parser_parse_castexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_CASTEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_peektoken(srcfile, 0);
    if
    (
        tkn->type == LEXER_TOKENTYPE_OPENPARENTH &&
        (tkn+1)->type == LEXER_TOKENTYPE_IDENTIFIER &&
        (tkn+2)->type == LEXER_TOKENTYPE_CLOSEPARENTH
    )
    {
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <cast-expression> ::= \
                           ( <type-name> ) <cast-expression>");
    }

    child = parser_parse_unaryexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    return node;
}

/*
    <multiplicative-expression> ::= <cast-expression>
                                  | <multiplicative-expression> * <cast-expression>
                                  | <multiplicative-expression> / <cast-expression>
                                  | <multiplicative-expression> % <cast-expression>
*/
parser_astnode_t* parser_parse_multiplicativeexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_MULTEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_castexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    tkn = parser_parse_peektoken(srcfile, 0);
    while
    (
        (tkn = parser_parse_peektoken(srcfile, 0)) && 
        (
            tkn->type == LEXER_TOKENTYPE_ASTERISK || 
            tkn->type == LEXER_TOKENTYPE_DIV ||
            tkn->type == LEXER_TOKENTYPE_MOD
        )
    )
    {
        tkn = parser_parse_consumetoken(srcfile);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_multiplicativeexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <additive-expression> ::= <multiplicative-expression>
                            | <additive-expression> + <multiplicative-expression>
                            | <additive-expression> - <multiplicative-expression>
*/
parser_astnode_t* parser_parse_additiveexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_ADDEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_multiplicativeexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    while
    (
        (tkn = parser_parse_peektoken(srcfile, 0)) && 
        (
            tkn->type == LEXER_TOKENTYPE_PLUS || 
            tkn->type == LEXER_TOKENTYPE_MINUS
        )
    )
    {
        tkn = parser_parse_consumetoken(srcfile);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_multiplicativeexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <shift-expression> ::= <additive-expression>
                         | <shift-expression> << <additive-expression>
                         | <shift-expression> >> <additive-expression>
*/
parser_astnode_t* parser_parse_shiftexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_SHIFTEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_additiveexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITSHIFTL)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <shift-expression> ::= \
                           <shift-expression> << <additive-expression>");

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITSHIFTR)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <shift-expression> ::= \
                           <shift-expression> >> <additive-expression>");

    return node;
}

/*
    <relational-expression> ::= <shift-expression>
                              | <relational-expression> < <shift-expression>
                              | <relational-expression> > <shift-expression>
                              | <relational-expression> <= <shift-expression>
                              | <relational-expression> >= <shift-expression>
*/
parser_astnode_t* parser_parse_relationalexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_RELATIONALEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_shiftexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_LESS)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <relational-expression> ::= \
                           <relational-expression> < <shift-expression>");

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_GREATER)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <relational-expression> ::= \
                           <relational-expression> > <shift-expression>");
    
    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_LE)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <relational-expression> ::= \
                           <relational-expression> <= <shift-expression>");

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_GE)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <relational-expression> ::= \
                           <relational-expression> >= <shift-expression>");

    return node;
}

/*
    <equality-expression> ::= <relational-expression>
                            | <equality-expression> == <relational-expression>
                            | <equality-expression> != <relational-expression>
*/
parser_astnode_t* parser_parse_equalityexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_EQUALITYEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_relationalexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_EQUAL)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <equality-expression> ::= \
                           <equality-expression> == <relational-expression>");

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_NOTEQUAL)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <equality-expression> ::= \
                           <equality-expression> != <relational-expression>");

    return node;
}

/*
    <and-expression> ::= <equality-expression>
                       | <and-expression> & <equality-expression>
*/
parser_astnode_t* parser_parse_andexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_ANDEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_equalityexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_AMPERSAND)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <and-expression> ::= \
                           <and-expression> & <equality-expression>");

    return node;
}

/*
    <exclusive-or-expression> ::= <and-expression>
                                | <exclusive-or-expression> ^ <and-expression>
*/
parser_astnode_t* parser_parse_exclusiveorexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_EXCLUSIVEOREXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_andexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITXOR)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <exclusive-or-expression> ::= \
                           <exclusive-or-expression> ^ <and-expression>");

    return node;
}

/*
    <inclusive-or-expression> ::= <exclusive-or-expression>
                                | <inclusive-or-expression> | <exclusive-or-expression>
*/
parser_astnode_t* parser_parse_inclusiveorexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_INCLUSIVEOREXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_exclusiveorexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITOR)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <inclusive-or-expression> ::= \
                           <inclusive-or-expression> | <exclusive-or-expression>");

    return node;
}

/*
    <logical-and-expression> ::= <inclusive-or-expression>
                               | <logical-and-expression> && <inclusive-or-expression>
*/
parser_astnode_t* parser_parse_logicalandexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_LOGICALANDEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_inclusiveorexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_AND)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <logical-and-expression> ::= \
                           <logical-and-expression> && <inclusive-or-expression>");

    return node;
}

/*
    <logical-or-expression> ::= <logical-and-expression>
                              | <logical-or-expression> || <logical-and-expression>
*/
parser_astnode_t* parser_parse_logicalorexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_LOGICALOREXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_logicalandexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OR)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <logical-or-expression> ::= \
                           <logical-or-expression> || <logical-and-expression>");

    return node;
}

/*
    <conditional-expression> ::= <logical-or-expression>
                               | <logical-or-expression> ? <expression> : <conditional-expression>
*/
parser_astnode_t* parser_parse_conditionalexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_CONDITIONALEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_logicalorexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_QUESTION)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), 
                           "TODO: <conditional-expression> ::= \
                           <logical-or-expression> ? <expression> : <conditional-expression>");

    return node;
}

/*
    <assignment-operator> ::= =
                            | *=
                            | /=
                            | %=
                            | +=
                            | -=
                            | <<=
                            | >>=
                            | &=
                            | ^=
                            | |=
*/
parser_astnode_t* parser_parse_assignmentoperator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_COMPOUNDSTATEMENT;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_consumetoken(srcfile);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_ASSIGN:
    case LEXER_TOKENTYPE_MULTEQUALS:
    case LEXER_TOKENTYPE_DIVEQUALS:
    case LEXER_TOKENTYPE_MODEQUALS:
    case LEXER_TOKENTYPE_PLUSEQUALS:
    case LEXER_TOKENTYPE_MINUSEQUALS:
    case LEXER_TOKENTYPE_BITSHIFTLEQ:
    case LEXER_TOKENTYPE_BITSHIFTREQ:
    case LEXER_TOKENTYPE_BITANDEQ:
    case LEXER_TOKENTYPE_BITXOREQ:
    case LEXER_TOKENTYPE_BITOREQ:
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = child;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        parser_parse_panic(srcfile, tkn, "expected assignment operator");
    }

    return node;
}

/*
    <assignment-expression> ::= <conditional-expression>
                              | <unary-expression> <assignment-operator> <assignment-expression>
*/
parser_astnode_t* parser_parse_assignmentexpression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    unsigned long int before;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_ASSIGNEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    before = srcfile->ast.curtok;
    child = parser_parse_conditionalexpression(srcfile, node, panic);
    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_ASSIGN || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_MULTEQUALS || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_DIVEQUALS || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_MODEQUALS ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_PLUSEQUALS ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_MINUSEQUALS || 
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITSHIFTLEQ ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITSHIFTREQ ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITANDEQ ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITXOREQ ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BITOREQ
    )
    {
        /* assignment op */

        parser_freenode(child);
        srcfile->ast.curtok = before;

        child = parser_parse_unaryexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        child = parser_parse_assignmentoperator(srcfile, node, panic);
        LIST_PUSH(node->children, child);

        child = parser_parse_assignmentexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }
    else
        LIST_PUSH(node->children, child);

    return node;
}

/*
    <expression> ::= <assignment-expression>
                   | <expression> , <assignment-expression>
*/
parser_astnode_t* parser_parse_expression(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_ASSIGNEXPR;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_assignmentexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    while(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_COMMA)
    {
        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_COMMA);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        child = parser_parse_assignmentexpression(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    return node;
}

/*
    <jump-statement> ::= goto <identifier> ;
                       | continue ;
                       | break ;
                       | return {<expression>}? ;
*/
parser_astnode_t* parser_parse_jumpstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_JUMPSTATEMENT;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_consumetoken(srcfile);
    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_GOTO:
    case LEXER_TOKENTYPE_CONTINUE:
    case LEXER_TOKENTYPE_BREAK:
    case LEXER_TOKENTYPE_RETURN:
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        parser_parse_panic(srcfile, tkn, "expected jump statement");
    }

    switch(tkn->type)
    {
    case LEXER_TOKENTYPE_GOTO:
        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_IDENTIFIER);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_SEMICOLON);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    case LEXER_TOKENTYPE_CONTINUE:
    case LEXER_TOKENTYPE_BREAK:
        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_SEMICOLON);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    case LEXER_TOKENTYPE_RETURN:
        if(parser_parse_peektoken(srcfile, 0)->type != LEXER_TOKENTYPE_SEMICOLON)
        {
            child = parser_parse_expression(srcfile, node, panic);
            LIST_PUSH(node->children, child);
        }

        tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_SEMICOLON);
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_TERMINAL;
        child->parent = node;
        child->token = tkn;
        LIST_PUSH(node->children, child);

        break;
    default:
        break; /* ? */
    }

    return node;
}

parser_astnode_t* parser_parse_iterationstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "TODO: <iteration-statement>");

    return NULL;
}

parser_astnode_t* parser_parse_selectionstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "TODO: <selection-statement>");

    return NULL;
}

/*
    <expression-statement> ::= {<expression>}? ;
*/
parser_astnode_t* parser_parse_expressionstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_COMPOUNDSTATEMENT;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    if(parser_parse_peektoken(srcfile, 0)->type != LEXER_TOKENTYPE_SEMICOLON)
    {
        child = parser_parse_expression(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_SEMICOLON);
    child = parser_parse_allocnode();
    child->type = PARSER_NODETYPE_TERMINAL;
    child->parent = child;
    child->token = tkn;
    LIST_PUSH(node->children, child);

    return node;
}

parser_astnode_t* parser_parse_labeledstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "TODO: <labeled-statement>");

    return NULL;
}

/*
    <statement> ::= <labeled-statement>
                  | <expression-statement>
                  | <compound-statement>
                  | <selection-statement>
                  | <iteration-statement>
                  | <jump-statement>
*/
parser_astnode_t* parser_parse_statement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_STATEMENT;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    if
    (
        (
            parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IDENTIFIER && 
            parser_parse_peektoken(srcfile, 1)->type == LEXER_TOKENTYPE_COLON
        ) ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_CASE ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_DEFAULT
    )
    {
        /* label statement */
        
        child = parser_parse_labeledstatement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
        return node;
    }
    
    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OPENBRACE)
    {
        /* compound statement */

        child = parser_parse_compoundstatement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
        return node;
    }

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_IF ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_SWITCH
    )
    {
        /* selection statement */
        
        child = parser_parse_selectionstatement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
        return node;
    }

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_WHILE ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_DO ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_FOR
    )
    {
        /* iteration statement */
        
        child = parser_parse_iterationstatement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
        return node;
    }

    if
    (
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_GOTO ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_CONTINUE ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_BREAK ||
        parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_RETURN
    )
    {
        /* jump statement */
        
        child = parser_parse_jumpstatement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
        return node;
    }

    /* if all else fails, assume it's an expression since thats hard to lookahead */
    child = parser_parse_expressionstatement(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    return node;
}

/*
    <initializer> ::= <assignment-expression>
                    | { <initializer-list> }
                    | { <initializer-list> , }
*/
parser_astnode_t* parser_parse_initializer(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_INITIALIZER;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OPENBRACE)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "TODO: { <initializer-list> }");

    child = parser_parse_assignmentexpression(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    return node;
}

/*
    <init-declarator> ::= <declarator>
                        | <declarator> = <initializer>
*/
parser_astnode_t* parser_parse_initdeclarator(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_INITDECL;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    child = parser_parse_declarator(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    tkn = parser_parse_peektoken(srcfile, 0);
    if(tkn->type != LEXER_TOKENTYPE_ASSIGN)
        return node;

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_ASSIGN);
    child = parser_parse_allocnode();
    child->type = PARSER_NODETYPE_TERMINAL;
    child->parent = node;
    child->token = tkn;
    LIST_PUSH(node->children, child);

    child = parser_parse_initializer(srcfile, node, panic);
    LIST_PUSH(node->children, child);

    return node;
}

/*
    <declaration> ::=  {<declaration-specifier>}+ {<init-declarator>}* ;
*/
parser_astnode_t* parser_parse_declaration(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    int i;

    parser_astnode_t *node, *child, *storagespec, *declarator;
    lexer_token_t *tkn;
    list_parser_astnode_p_t declspecs;
    parser_typedef_t typedefel;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_DECL;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    declspecs = parser_parse_consumedeclspecs(srcfile, node);
    if(!declspecs.size)
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "expected declaration specifier");

    for(i=0; i<declspecs.size; i++)
    {
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_DECLSPEC;
        LIST_INITIALIZE(child->children);
        child->parent = node;

        LIST_RESIZE(child->children, 1);
        declspecs.data[i]->parent = child;
        child->children.data[0] = declspecs.data[i];

        LIST_PUSH(node->children, child);
    }

    child = parser_parse_initdeclarator(srcfile, node, panic);
    declarator = child->children.data[0];
    LIST_PUSH(node->children, child);

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_SEMICOLON);
    child = parser_parse_allocnode();
    child->type = PARSER_NODETYPE_TERMINAL;
    child->parent = node;
    child->token = tkn;
    LIST_PUSH(node->children, child);

    for(i=0, storagespec=NULL; i<declspecs.size; i++)
    {
        if(declspecs.data[i]->type != PARSER_NODETYPE_STORAGECLASSSPEC)
            continue;

        storagespec = declspecs.data[i]->children.data[0];
    }

    if(!storagespec)
        return node;

    /* this is a typedef declaration */

    memset(&typedefel, 0, sizeof(typedefel));
    typedefel.name = parser_parse_getdeclaratorname(declarator);
    typedefel.declaration = node;
    HASHMAP_SET(srcfile->ast.typedefs, typedefel.name, typedefel);

    return node;
}

/*
    <compound-statement> ::= { {<declaration>}* {<statement>}* }
*/
parser_astnode_t* parser_parse_compoundstatement(srcfile_t* srcfile, parser_astnode_t* parent, bool panic)
{
    parser_astnode_t *node, *child;
    lexer_token_t *tkn;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_COMPOUNDSTATEMENT;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_OPENBRACE);
    child = parser_parse_allocnode();
    child->type = PARSER_NODETYPE_TERMINAL;
    child->parent = child;
    child->token = tkn;
    LIST_PUSH(node->children, child);

    while(parser_parse_istokendeclspec(parser_parse_peektoken(srcfile, 0)))
    {
        if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_EOF)
            parser_parse_panic(srcfile, tkn, "unterminated compound statement");

        child = parser_parse_declaration(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    while(parser_parse_peektoken(srcfile, 0)->type != LEXER_TOKENTYPE_CLOSEBRACE)
    {
        if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_EOF)
            parser_parse_panic(srcfile, tkn, "unterminated compound statement");

        child = parser_parse_statement(srcfile, node, panic);
        LIST_PUSH(node->children, child);
    }

    tkn = parser_parse_expecttoken(srcfile, LEXER_TOKENTYPE_CLOSEBRACE);
    child = parser_parse_allocnode();
    child->type = PARSER_NODETYPE_TERMINAL;
    child->parent = child;
    child->token = tkn;
    LIST_PUSH(node->children, child);

    return node;
}

/*
    <function-definition> ::= {<declaration-specifier>}* <declarator> <compound-statement>
                                                                      ^ we start here in this function
*/
parser_astnode_t* parser_parse_functiondefinition(srcfile_t* srcfile, parser_astnode_t* parent, list_parser_astnode_p_t declspecs, parser_astnode_t* declarator, bool panic)
{
    int i;

    parser_astnode_t *node, *child;

    node = parser_parse_allocnode();
    node->type = PARSER_NODETYPE_FUNCTIONDEF;
    LIST_INITIALIZE(node->children);
    node->parent = parent;

    for(i=0; i<declspecs.size; i++)
    {
        child = parser_parse_allocnode();
        child->type = PARSER_NODETYPE_DECLSPEC;
        LIST_INITIALIZE(child->children);
        child->parent = node;

        LIST_RESIZE(child->children, 1);
        declspecs.data[i]->parent = child;
        child->children.data[0] = declspecs.data[i];

        LIST_PUSH(node->children, child);
    }

    LIST_PUSH(node->children, declarator);

    child = parser_parse_compoundstatement(srcfile, node, panic);
    LIST_PUSH(node->children, child);

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
    unsigned long int begin;

    newnode = declarator = 0;

    newnode = malloc(sizeof(parser_astnode_t));
    newnode->type = PARSER_NODETYPE_EXTERNALDECL;
    LIST_INITIALIZE(newnode->children);
    newnode->parent = parent;
    newnode->token = NULL;

    begin = srcfile->ast.curtok;

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

        srcfile->ast.curtok = begin;
        child = parser_parse_declaration(srcfile, newnode, panic);
        LIST_PUSH(newnode->children, child);
    }
    else if(parser_parse_peektoken(srcfile, 0)->type == LEXER_TOKENTYPE_OPENBRACE)
    {
        /* function definition */

        child = parser_parse_functiondefinition(srcfile, newnode, declspecs, declarator, panic);
        LIST_PUSH(newnode->children, child);
    }
    else
    {
        parser_parse_panic(srcfile, parser_parse_peektoken(srcfile, 0), "expected ';'");
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
        child = parser_parse_externaldecl(srcfile, root, panic);
        LIST_PUSH(root->children, child);
    }

    return root;
}

void parser_parse(struct srcfile_s* srcfile)
{
    assert(srcfile);
    
    profiler_push("Parse");

    HASHMAP_INITIALIZE(srcfile->ast.typedefs, HASHMAP_BUCKETS_LARGE, string_parser_typedef);
    srcfile->ast.nodes = parser_parse_translationunit(srcfile, true);
    parser_print(&srcfile->ast);
    
    profiler_pop();
}
