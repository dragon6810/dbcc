#ifndef _parser_h
#define _parser_h

#include <frontend/lexer/lexer.h>
#include <std/hashmap/hashmap.h>
#include <std/list/list_types.h>

/*
 * ================================
 *       FORWARD DECLARATIONS
 * ================================
*/

struct srcfile_s;

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct parser_astnode_s parser_astnode_t;
typedef struct parser_tree_s parser_tree_t;
typedef struct parser_typedef_s parser_typedef_t;

typedef enum
{
    PARSER_NODETYPE_TERMINAL=0,

    PARSER_NODETYPE_TRANSLATIONUNIT,
    PARSER_NODETYPE_EXTERNALDECL,
    PARSER_NODETYPE_FUNCTIONDEF,
    PARSER_NODETYPE_DECLSPEC,
    PARSER_NODETYPE_STORAGECLASSSPEC,
    PARSER_NODETYPE_TYPESPEC,
    PARSER_NODETYPE_STRUCTORUNIONSPEC,
    PARSER_NODETYPE_STRUCTORUNION,
    PARSER_NODETYPE_STRUCTDECL,
    PARSER_NODETYPE_DECLARATOR,
    PARSER_NODETYPE_POINTER,
    PARSER_NODETYPE_TYPEQUALIFIER,
    PARSER_NODETYPE_DIRECTDECL,
    PARSER_NODETYPE_CONSTEXPR,
    PARSER_NODETYPE_CONDITIONALEXPR,
    PARSER_NODETYPE_LOGICALOREXPR,
    PARSER_NODETYPE_LOGICALANDEXPR,
    PARSER_NODETYPE_INCLUSIVEOREXPR,
    PARSER_NODETYPE_EXCLUSIVEOREXPR,
    PARSER_NODETYPE_ANDEXPR,
    PARSER_NODETYPE_EQUALITYEXPR,
    PARSER_NODETYPE_RELATIONALEXPR,
    PARSER_NODETYPE_SHIFTEXPR,
    PARSER_NODETYPE_ADDEXPR,
    PARSER_NODETYPE_MULTEXPR,
    PARSER_NODETYPE_CASTEXPR,
    PARSER_NODETYPE_UNARYEXPR,
    PARSER_NODETYPE_POSTFIXEXPR,
    PARSER_NODETYPE_PRIMARYEXPR,
    PARSER_NODETYPE_CONSTANT,
    PARSER_NODETYPE_EXPR,
    PARSER_NODETYPE_ASSIGNEXPR,
    PARSER_NODETYPE_ASSIGNOP,
    PARSER_NODETYPE_UNARYOP,
    PARSER_NODETYPE_TYPENAME,
    PARSER_NODETYPE_PARAMTYPELIST,
    PARSER_NODETYPE_PARAMLIST,
    PARSER_NODETYPE_PARAMDECL,
    PARSER_NODETYPE_ABSTRACTDECL,
    PARSER_NODETYPE_DIRECTABSTRACTDECL,
    PARSER_NODETYPE_ENUMSPEC,
    PARSER_NODETYPE_ENUMLIST,
    PARSER_NODETYPE_ENUM,
    PARSER_NODETYPE_TYPEDEFNAME,
    PARSER_NODETYPE_DECL,
    PARSER_NODETYPE_INITDECL,
    PARSER_NODETYPE_INITIALIZER,
    PARSER_NODETYPE_INITIALIZERLIST,
    PARSER_NODETYPE_COMPOUNDSTATEMENT,
    PARSER_NODETYPE_STATEMENT,
    PARSER_NODETYPE_LABELEDSTATEMENT,
    PARSER_NODETYPE_EXPRSTATEMENT,
    PARSER_NODETYPE_SELECTIONSTATEMENT,
    PARSER_NODETYPE_ITERATIONSTATEMENT,
    PARSER_NODETYPE_JUMPSTATEMENT,
} parser_nodetype_e;

LIST_TYPE(parser_astnode_t, list_parser_astnode)
LIST_TYPE(parser_astnode_t*, list_parser_astnode_p)
HASHMAP_TYPE_DECL(char*, parser_typedef_t, string_parser_typedef)

struct parser_astnode_s
{
    parser_nodetype_e type;
    list_parser_astnode_p_t children;
    parser_astnode_t* parent;
    lexer_token_t* token; /* NULL if non-terminal */
};

struct parser_tree_s
{
    parser_astnode_t* nodes;
    hashmap_string_parser_typedef_t typedefs;

    unsigned long int curtok;
};

struct parser_typedef_s
{
    char *name;
    parser_astnode_t* declaration;
};

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void parser_parse(struct srcfile_s* srcfile);
void parser_freenode(parser_astnode_t* node);
void parser_print(parser_tree_t* tree);
void parser_writemermaid(parser_tree_t* tree, const char* path);
void parser_typetostr(parser_nodetype_e type, char* out);

#endif