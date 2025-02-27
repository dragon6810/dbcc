#include <frontend/parser/parser.h>

#include <string.h>

#include <std/assert/assert.h> 

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

    parser_typetostr(node->type, name);
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