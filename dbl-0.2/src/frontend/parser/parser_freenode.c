#include <frontend/parser/parser.h>

#include <std/assert/assert.h>

void parser_freenode_r(parser_astnode_t* node)
{
    int i;

    for(i=0; i<node->children.size; i++)
    {
        if(!node->children.data[i]->children.size)
        {
            free(node->children.data[i]);
            continue;
        }

        parser_freenode_r(node->children.data[i]);
        free(node);
    }

    LIST_FREE(node->children);
    free(node);
}

void parser_freenode(parser_astnode_t* node)
{
    assert(node);

    parser_freenode_r(node);
}