#include <backend/register/register.h>

#include <std/assert/assert.h>

// find any node we interfere with but don't rely on.
int register_colorgraph_findspillcol(register_node_t* node)
{
    register_edge_t *inter, *rely;

    assert(node);

    for(inter=node->interfere; inter; inter=inter->next)
    {
        if(inter->node->color <= 0)
            continue;

        for(rely=node->rely; rely; rely=rely->next)
        {
            if(rely == inter)
                break;
        }

        if(rely)
            continue;

        return inter->node->color;
    }

    assert(0 && "no valid spill register found!");

    return -1;
}

void register_colorgraph_colornode(int ncol, register_node_t* node)
{
    int i;
    register_edge_t *edge;

    bool colors[ncol];

    assert(node);

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
        node->color = register_colorgraph_findspillcol(node);
        return;
    }

    node->color = i;
}

void register_colorgraph(int nreg, register_node_t** graph, int ncol)
{
    int i;

    for(i=0; i<nreg; i++)
        graph[i]->color = -1;

    for(i=0; i<nreg; i++)
        register_colorgraph_colornode(ncol, graph[i]);
}