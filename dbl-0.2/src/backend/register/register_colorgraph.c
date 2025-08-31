#include <backend/register/register.h>

#include <std/assert/assert.h>

void register_colorgraph_colornode(int ncol, register_node_t* node)
{
    int i;
    register_edge_t *edge;

    bool colors[ncol];

    for(i=0; i<ncol; i++)
        colors[i] = true;

    for(edge=node->interfere; edge; edge=edge->next)
    {
        if(edge->node->color < 0)
            continue;

        colors[edge->node->color] = false;
    }

    for(i=0; i<ncol; i++)
        if(colors[i])
            break;
    if(i >= ncol)
    {
        printf("TODO: spill\n");
        exit(1);
    }

    node->color = i;
}

void register_colorgraph(int nreg, register_node_t** graph, int ncol)
{
    int i;

    for(i=0; i<nreg; i++)
        graph[i]->color = -1;

    for(i=0; i<nreg; i++)
        register_colornode(ncol, graph[i]);
}