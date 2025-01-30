#include "lexer_bnf.h"

#include <string.h>

#include "error.h"

char* tkn_bnf_skipwhitespace(char* str)
{
    while(*str<=32)
        str++;

    return str;
}

uint64_t tkn_bnf_parsenode(bnf_spec_node_t* node, char* input, uint64_t* pos, darr_t* tokens)
{
    int i, j;

    uint64_t len;
    tkn_bnf_token_t tkn;
    darr_t *curarr;
    bnf_spec_edge_t *curedge;
    uint64_t before;
    bool all;

    char *start;

    if(node->def)
    {
        len = strlen(node->def);
        if(strncmp(node->def, input + *pos, len))
            return 0;

        tkn.ichar = *pos;
        tkn.leaf = node;
        tkn.payload = node->def;
        printf("token of default type \"%s\": \"%s\".\n", node->def, tkn.payload);
        return 1;
    }

    if(node->dyn)
    {
        start = input + *pos;
        tkn.ichar = *pos;
        len = strlen(input);
        while(*pos < len && input[(*pos)++] > 32);
        (*pos)--;
    
        tkn.payload = malloc(*pos + input - start + 1);
        memcpy(tkn.payload, start, *pos + input - start);
        tkn.payload[*pos + input - start] = 0;
        tkn.leaf = node;

        printf("token of dynamic type \"%s\": \"%s\".\n", node->dyn, tkn.payload);

        // skipwhitespace
        while(*pos < len && input[(*pos)++] <= 32);
        *(pos)--;
        return 1;
    }

    for(i=0, curarr=node->children.data; i<node->children.len; i++, curarr++)
    {
        all = true;
        before = *pos;
        for(j=0, curedge=curarr->data; j<curarr->len; j++)
        {
            if(!tkn_bnf_parsenode(curedge->node, input, pos, tokens))
            {
                *pos = before;
                all = false;
                break;
            }
        }

        if(all)
            break;
    }
}

tkn_bnf_tokenfile_t tkn_bnf_loadfile(char* filename)
{
    tkn_bnf_tokenfile_t tknfile;

    FILE* ptr;
    unsigned long int fsize;
    char *buff;
    darr_t parents;
    uint64_t pos;
    darr_t tokens;

    ptr = fopen(filename, "r");
    if(!ptr)
        Error("can't open source file \"%s\".\n", filename);

    printf("opening source file \"%s\".\n", filename);
    tknfile.filename = strdup(filename);

    fseek(ptr, 0, SEEK_END);
    fsize = ftell(ptr);
    buff = malloc(fsize + 1);
    fseek(ptr, 0, SEEK_SET);
    fread(buff, 1, fsize, ptr);
    fclose(ptr);
    buff[fsize] = 0;

    pos = 0;
    darr_init(&tokens, sizeof(tkn_bnf_token_t));
    tkn_bnf_parsenode(bnf_spec.nodes.data, buff, &pos, &tokens);

    free(buff);
    return tknfile;
}