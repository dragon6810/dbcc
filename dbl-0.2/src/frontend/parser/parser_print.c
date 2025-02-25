#include <frontend/parser/parser.h>

#include <string.h>

#include <std/assert/assert.h>

void parser_print_typetostr(parser_nodetype_e type, char* out)
{
    assert(out);

    switch(type)
    {
    case PARSER_NODETYPE_TRANSLATIONUNIT:
        strcpy(out, "translation unit");
        break;
    case PARSER_NODETYPE_EXTERNALDECL:
        strcpy(out, "external declaration");
        break;
    case PARSER_NODETYPE_FUNCTIONDEF:
        strcpy(out, "function definition");
        break;
    case PARSER_NODETYPE_DECLSPEC:
        strcpy(out, "declaration specifier");
        break;
    case PARSER_NODETYPE_STORAGECLASSSPEC:
        strcpy(out, "storage class specifier");
        break;
    case PARSER_NODETYPE_TYPESPEC:
        strcpy(out, "type specifier");
        break;
    case PARSER_NODETYPE_STRUCTORUNIONSPEC:
        strcpy(out, "struct or union specifier");
        break;
    case PARSER_NODETYPE_STRUCTORUNION:
        strcpy(out, "struct or union");
        break;
    case PARSER_NODETYPE_STRUCTDECL:
        strcpy(out, "struct declaration");
        break;
    case PARSER_NODETYPE_DECLARATOR:
        strcpy(out, "declarator");
        break;
    case PARSER_NODETYPE_POINTER:
        strcpy(out, "pointer");
        break;
    case PARSER_NODETYPE_TYPEQUALIFIER:
        strcpy(out, "type qualifier");
        break;
    case PARSER_NODETYPE_DIRECTDECL:
        strcpy(out, "direct declarator");
        break;
    case PARSER_NODETYPE_CONSTEXPR:
        strcpy(out, "constant expression");
        break;
    case PARSER_NODETYPE_CONDITIONALEXPR:
        strcpy(out, "conditional expression");
        break;
    case PARSER_NODETYPE_LOGICALOREXPR:
        strcpy(out, "logical or expression");
        break;
    case PARSER_NODETYPE_LOGICALANDEXPR:
        strcpy(out, "logical and expression");
        break;
    case PARSER_NODETYPE_INCLUSIVEOREXPR:
        strcpy(out, "inclusive or expression");
        break;
    case PARSER_NODETYPE_EXCLUSIVEOREXPR:
        strcpy(out, "exclusive or expression");
        break;
    case PARSER_NODETYPE_ANDEXPR:
        strcpy(out, "and expression");
        break;
    case PARSER_NODETYPE_EQUALITYEXPR:
        strcpy(out, "equality expression");
        break;
    case PARSER_NODETYPE_RELATIONALEXPR:
        strcpy(out, "relational expression");
        break;
    case PARSER_NODETYPE_SHIFTEXPR:
        strcpy(out, "shift expression");
        break;
    case PARSER_NODETYPE_ADDEXPR:
        strcpy(out, "add expression");
        break;
    case PARSER_NODETYPE_MULTEXPR:
        strcpy(out, "multiplication expression");
        break;
    case PARSER_NODETYPE_CASTEXPR:
        strcpy(out, "cast expression");
        break;
    case PARSER_NODETYPE_UNARYEXPR:
        strcpy(out, "unary expression");
        break;
    case PARSER_NODETYPE_POSTFIXEXPR:
        strcpy(out, "postfix expression");
        break;
    case PARSER_NODETYPE_PRIMARYEXPR:
        strcpy(out, "primary expression");
        break;
    case PARSER_NODETYPE_CONSTANT:
        strcpy(out, "constant");
        break;
    case PARSER_NODETYPE_EXPR:
        strcpy(out, "expression");
        break;
    case PARSER_NODETYPE_ASSIGNEXPR:
        strcpy(out, "assignment expression");
        break;
    case PARSER_NODETYPE_ASSIGNOP:
        strcpy(out, "assignment operator");
        break;
    case PARSER_NODETYPE_UNARYOP:
        strcpy(out, "unary operator");
        break;
    case PARSER_NODETYPE_TYPENAME:
        strcpy(out, "typename");
        break;
    case PARSER_NODETYPE_PARAMTYPELIST:
        strcpy(out, "parameter type list");
        break;
    case PARSER_NODETYPE_PARAMLIST:
        strcpy(out, "parameter list");
        break;
    case PARSER_NODETYPE_PARAMDECL:
        strcpy(out, "parameter declaration");
        break;
    case PARSER_NODETYPE_ABSTRACTDECL:
        strcpy(out, "abstract declarator");
        break;
    case PARSER_NODETYPE_DIRECTABSTRACTDECL:
        strcpy(out, "direct abstract declarator");
        break;
    case PARSER_NODETYPE_ENUMSPEC:
        strcpy(out, "enumerator specifier");
        break;
    case PARSER_NODETYPE_ENUMLIST:
        strcpy(out, "enumerator list");
        break;
    case PARSER_NODETYPE_ENUM:
        strcpy(out, "enumerator");
        break;
    case PARSER_NODETYPE_TYPEDEFNAME:
        strcpy(out, "typedef name");
        break;
    case PARSER_NODETYPE_DECL:
        strcpy(out, "declaration");
        break;
    case PARSER_NODETYPE_INITDECL:
        strcpy(out, "init declaration");
        break;
    case PARSER_NODETYPE_INITIALIZER:
        strcpy(out, "initializer");
        break;
    case PARSER_NODETYPE_INITIALIZERLIST:
        strcpy(out, "initializer list");
        break;
    case PARSER_NODETYPE_COMPOUNDSTATEMENT:
        strcpy(out, "compound statement");
        break;
    case PARSER_NODETYPE_STATEMENT:
        strcpy(out, "statement");
        break;
    case PARSER_NODETYPE_LABELEDSTATEMENT:
        strcpy(out, "labeled statement");
        break;
    case PARSER_NODETYPE_EXPRSTATEMENT:
        strcpy(out, "expression statement");
        break;
    case PARSER_NODETYPE_SELECTIONSTATEMENT:
        strcpy(out, "selection statement");
        break;
    case PARSER_NODETYPE_ITERATIONSTATEMENT:
        strcpy(out, "iteration statement");
        break;
    case PARSER_NODETYPE_JUMPSTATEMENT:
        strcpy(out, "jump statement");
        break;
    default:
        strcpy(out, "unkown");
        break;
    }
}

void parser_print_r(parser_astnode_t* node, int depth, bool last, char* prefix)
{
    int i;

    char name[LEXER_MAXHARDTOKENLEN];
    char *newprefix;

    printf("\033[32m%s%s\033[0m", prefix, (depth > 0 ? (last ? "`-" : "|-") : ""));

    if(!node)
    {
        printf("\033[32mNULL\033[0m\n");
        return;
    }

    if(node->token)
    {
        lexer_tkntypetostring(node->token->type, name);
        printf("\033[33mTerminal \"%s\" (\"%s\", %s:%lu:%lu)\033[0m\n", node->token->val, name, node->token->file, node->token->line+1, node->token->col+1);
        return;
    }

    parser_print_typetostr(node->type, name);
    printf("\033[32mNon-Terminal (\"%s\")\033[0m\n", name);

    newprefix = malloc(strlen(prefix) + 1  + 1 + 1);
    sprintf(newprefix, "%s%s ", prefix, (last ? " " : "|"));

    for(i=0; i<node->children.size; i++)
        parser_print_r(node->children.data[i], depth+1, i==node->children.size-1, newprefix);

    free(newprefix);
}

void parser_print(parser_tree_t* tree)
{
    assert(tree);

    parser_print_r(tree->nodes, 0, true, "");
}