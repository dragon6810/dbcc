#include "bnf.h"

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "textutils.h"

bnf_spec_tree_t bnf_spec;

char* bnf_nexttoken(FILE* ptr)
{
    char c;
    bool either;

    char* str;

    unsigned long int start;
    unsigned long int end;

    either = true;
    while(either)
    {
        either = false;
        
        if(txt_skipwhitespace(ptr))
            either = true;
        
        if((c = fgetc(ptr)) == '#' && linestart)
        {
            txt_skipline(ptr);
            either = true;
        }
        else if(c != EOF)
            fseek(ptr, -1, SEEK_CUR);
    
        if(txt_skipwhitespace(ptr))
            either = true;
    }
    
    start = ftell(ptr);    
    fseek(ptr, 0, SEEK_END);
    if(ftell(ptr) == start)
    {
        str = calloc(1, 1);
        return str;
    }

    fseek(ptr, start, SEEK_SET);
    while((c = fgetc(ptr)) != EOF && c > 32);
    if(c != EOF)
        fseek(ptr, -1, SEEK_SET);
    else
        printf("eof reached\n");
    end = ftell(ptr) - 1;

    str = malloc(end - start + 1);
    fseek(ptr, start, SEEK_SET);
    fread(str, 1, end - start, ptr);
    str[end - start] = 0;
    
    return str;
}

char* bnf_removeanglebrackets(char* str)
{
    char* newstr;
    unsigned long int start, end;

    start = 0;
    if(str[0] == '<')
        start++;

    end = strlen(str);
    if(str[end - 1] == '>')
        end--;

    newstr = malloc(end - start);
    memcpy(newstr, str + start, end - start);
    newstr[end - start] = 0;

    return newstr;
}

bool bnf_istokennoderef(darr_t tokens, int index)
{
    char** tokensdata;

    tokensdata = (char**) tokens.data;
    if(tokensdata[index][0] != '<' && !(tokensdata[index][0] == '{' && tokensdata[index][1] == '<'))
        return false;

    return true;
}

bool bnf_istokennodelabel(darr_t tokens, int index)
{
    char** tokendata;

    if(index >= tokens.len - 1)
       return false;

    tokendata = (char**) tokens.data;
    if(strcmp(tokendata[index + 1], "::="))
        return false;

    return bnf_istokennoderef(tokens, index);
}

bnf_spec_edge_t bnf_parseedge(bnf_spec_tree_t* tree, char* tkn)
{
    int i;
    bnf_spec_node_t *curnode;
    
    bnf_spec_node_t *nodesdata;
    bnf_spec_edge_t edge;
    bnf_spec_node_t newnode;
    char* searchname;
    unsigned long int start, end;

    if(tkn[0] == '<')
    {
        edge.min = edge.max = 1;
        searchname = bnf_removeanglebrackets(tkn);
    }
    if(tkn[0] == '{')
    {
        start = 1;
        end = strlen(tkn) - 1;
        
        edge.min = 0;
        edge.max = -1;
        if(tkn[end] != '}')
        {
            switch(tkn[end])
            {
            case '?':
                edge.min = 0;
                edge.max = 1;
                break;
            case '+':
                edge.min = 1;
                edge.max = -1;
                break;
            case '*':
                edge.min = 0;
                edge.max = -1;
                break;
            default:
                printf("WARNING: unrecognized bnf curly braces symbol '%c'.\n", tkn[end]);
                break;
            }
            end--;
        }

        // skip angle brackets, too
        start++;
        end--;

        searchname = malloc(end - start + 1);
        memcpy(searchname, tkn + start, end - start);
        searchname[end - start] = 0;
    }

    nodesdata = (bnf_spec_node_t*) tree->nodes.data;
    for(i=0, curnode=nodesdata; i<tree->nodes.len; i++, curnode++)
    {
        if(!curnode->symbol)
            continue;

        if(strcmp(curnode->symbol, searchname))
            continue;
    
        edge.node = curnode;
        free(searchname);
        return edge;
    }

    memset(&newnode, 0, sizeof(newnode));
    newnode.dyn = searchname;
    darr_push(&tree->nodes, &newnode);
    nodesdata = (bnf_spec_node_t*) tree->nodes.data;
    edge.node = nodesdata + tree->nodes.len - 1;
    
    return edge;
}

void bnf_edgepass(bnf_spec_tree_t* tree, darr_t tokens)
{
    int i, j;
    int startlen;

    bnf_spec_node_t *nodesdata, *curnode;
    bnf_spec_node_t newnode;
    char **tokendata, **curtkn;
    darr_t *curarr;
    darr_t newarr;
    bnf_spec_edge_t newedge;

    tokendata = (char**) tokens.data;
    startlen = tree->nodes.len;
    for(i=0; i<startlen; i++)
    {
        nodesdata = (bnf_spec_node_t*) tree->nodes.data;
        printf("Processing node \"%s\".\n", nodesdata[i].symbol); 
        curnode = nodesdata + i;
        
        darr_init(&newarr, sizeof(bnf_spec_edge_t));
        darr_push(&curnode->children, &newarr);

        if(j<0)
            continue;

        for(j=curnode->itkn+2; !bnf_istokennodelabel(tokens, j) && j<tokens.len; j++)
        {
            nodesdata = (bnf_spec_node_t*) tree->nodes.data;
            curnode = nodesdata + i;
            
            curarr = &((darr_t*)curnode->children.data)[curnode->children.len-1];

            curtkn = tokendata + j;
            
            if(!strcmp(*curtkn, "|"))
            {
                darr_init(&newarr, sizeof(bnf_spec_edge_t));
                darr_push(&curnode->children, &newarr);
                printf("\tOR\n");
                continue;
            }

            if(!bnf_istokennoderef(tokens, j))
            {
                memset(&newnode, 0, sizeof(newnode));
                newnode.def = malloc(strlen(*curtkn) + 1);
                strcpy(newnode.def, *curtkn);
                newnode.def[strlen(*curtkn)] = 0;
                newnode.itkn = -1;

                darr_push(&tree->nodes, &newnode);

                memset(&newedge, 0, sizeof(newedge));
                newedge.min = newedge.max = 1;
                newedge.node = &((bnf_spec_node_t*)tree->nodes.data)[tree->nodes.len-1];

                darr_push(curarr, &newedge);
                
                printf("\tDefault \"%s\".\n", newedge.node->def);
                continue;
            }

            newedge = bnf_parseedge(tree, *curtkn);
            darr_push(curarr, &newedge);
    
            if(newedge.node->symbol)
                printf("\tNode Ref \"%s\".\n", newedge.node->symbol);
            else if(newedge.node->dyn)
                printf("\tDynamic Ref \"%s\".\n", newedge.node->dyn);
        }
    }
}

bnf_spec_tree_t bnf_nodepass(darr_t tokens)
{
    int i;

    bnf_spec_tree_t tree;
    bnf_spec_node_t curnode;

    char** tokensdata = (char**) tokens.data;

    char* nodelabel;

    darr_init(&tree.nodes, sizeof(bnf_spec_node_t));
    for(i=0; i<tokens.len; i++)
    {
        if(!bnf_istokennodelabel(tokens, i))
            continue;

        nodelabel = bnf_removeanglebrackets(tokensdata[i]);
        printf("Node Label: \"%s\".\n", nodelabel);
        curnode.def = 0;
        curnode.symbol = nodelabel;
        darr_init(&curnode.children, sizeof(darr_t));
        curnode.itkn = i;
    
        darr_push(&tree.nodes, &curnode);
    }

    return tree; 
}

bool bnf_loadspec(char* filepath)
{
    int i;

    FILE* ptr;
    char* str;
    darr_t tokens; // Array of char*
    char** tokensdata;
    bnf_spec_tree_t tree;

    ptr = fopen(filepath, "r");
    if(!ptr)
    {
        Error("couldn't open bnf specification file \"%s\".\n", filepath);
        return false;
    }

    darr_init(&tokens, sizeof(char*));
    while(true)
    {
        str = bnf_nexttoken(ptr);
        if(!str[0])
            break;

        darr_push(&tokens, &str);
    }
    free(str);

    fclose(ptr);
   
    tree = bnf_nodepass(tokens);
    bnf_edgepass(&tree, tokens);
    bnf_spec = tree;

    tokensdata = (char**) tokens.data;
    for(i=0; i<tokens.len; i++)
        free(tokensdata[i]);

    free(tokensdata); 
        
    return true;
}
