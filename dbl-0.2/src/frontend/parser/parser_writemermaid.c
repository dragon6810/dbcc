#include <frontend/parser/parser.h>

#include <string.h>

#include <std/assert/assert.h>

int parser_writemermaid_r(FILE* ptr, parser_astnode_t* node, int nnodes)
{
    int i;

    int inode, ichild;
    char name[LEXER_MAXHARDTOKENLEN];
    char *valdup;

    assert(ptr);
    assert(node);

    inode = nnodes;

    if(node->type == PARSER_NODETYPE_TERMINAL)
    {
        lexer_tkntypetostring(node->token->type, name);
        valdup = strdup(node->token->val);
        for(i=0; i<strlen(valdup); i++)
            if(valdup[i] == '"') valdup[i] = '\'';
        fprintf(ptr, "  %d[\"Terminal %s (%s)\"]\n", nnodes++, valdup, name);
        free(valdup);
        return inode;
    }

    parser_typetostr(node->type, name);
    fprintf(ptr, "  %d[\"Non-Terminal %s\"]\n", nnodes++, name);

    for(i=0; i<node->children.size; i++)
    {
        ichild = nnodes;
        nnodes = parser_writemermaid_r(ptr, node->children.data[i], nnodes);
        fprintf(ptr, "  %d-->%d\n", inode, ichild);
    }

    return nnodes;
}

void parser_writemermaid(parser_tree_t* tree, const char* path)
{
    FILE *ptr;

    assert(tree);
    assert(path);

    ptr = fopen(path, "w");
    assert(ptr);

    fprintf(ptr, "graph TD\n");
    parser_writemermaid_r(ptr, tree->nodes, 0);
}