#include <backend/register/register.h>

#include <std/assert/assert.h>

static void register_makegraph_findreliance(ir_definition_function_t* func, register_node_t** nodes)
{
    int i;
    ir_instruction_t *cmd;
    
    int reg;
    int relies[2];
    register_edge_t *edge;

    for(cmd=func->instructions; cmd; cmd=cmd->next)
    {
        reg = -1;
        for(i=0; i<2; i++)
            relies[i] = -1;

        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            reg = cmd->add.dst.reg;
            relies[0] = cmd->add.a.reg;
            relies[1] = cmd->add.b.reg;
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            reg = cmd->mult.dst.reg;
            relies[0] = cmd->mult.a.reg;
            relies[1] = cmd->mult.b.reg;
            break;
        default:
            break;
        }

        if(reg < 0)
            continue;

        for(i=0; i<2; i++)
        {
            if(relies[i] < 0)
                break;
            
            edge = calloc(1, sizeof(register_edge_t));
            edge->node = nodes[relies[i]];
            edge->next = nodes[reg]->rely;
            nodes[reg]->rely = edge;
        }
    }
}

static void register_makegraph_findinterference(int nreg, register_node_t** nodes)
{
    int i, j;

    register_node_t *a, *b;
    register_edge_t *newedge;

    for(i=0; i<nreg; i++)
    {
        for(j=i; j<nreg; j++)
        {
            if(nodes[j]->life[0] > nodes[i]->life[1])
                continue;
            if(nodes[j]->life[1] < nodes[i]->life[0])
                continue;

            a = nodes[i];
            b = nodes[j];

            newedge = calloc(1, sizeof(register_edge_t));
            newedge->node = b;
            newedge->next = a->interfere;
            a->interfere = newedge;

            newedge = calloc(1, sizeof(register_edge_t));
            newedge->node = a;
            newedge->next = b->interfere;
            b->interfere = newedge;
        }
    }
}

static void register_makegraph_findlifetimes(ir_definition_function_t* func, register_node_t** nodes)
{
    int i, j;
    ir_instruction_t *cmd;

    int used[3];

    for(i=0; i<func->nregisters; i++)
        nodes[i]->life[0] = nodes[i]->life[1] = -1;
    
    for(cmd=func->instructions, i=0; cmd; cmd=cmd->next, i++)
    {
        for(j=0; j<3; j++)
            used[j] = -1;

        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            used[0] = cmd->loadconst.reg.reg;
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            used[0] = cmd->ret.reg.reg;
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            used[0] = cmd->add.dst.reg;
            used[1] = cmd->add.a.reg;
            used[2] = cmd->add.b.reg;
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            used[0] = cmd->mult.dst.reg;
            used[1] = cmd->mult.a.reg;
            used[2] = cmd->mult.b.reg;
            break;
        default:
            break;
        }

        for(j=0; j<3; j++)
        {
            if(used[j] < 0)
                break;
            
            if(nodes[used[j]]->life[0] < 0)
                nodes[used[j]]->life[0] = i;
            nodes[used[j]]->life[1] = i;
        }
    }
}

register_node_t** register_makegraph(ir_definition_function_t* func)
{
    int i;

    register_node_t *graph, *node, **nodes;

    assert(func);

    nodes = malloc(sizeof(register_node_t*) * func->nregisters);
    for(i=0; i<func->nregisters; i++)
    {
        node = calloc(1, sizeof(register_node_t));
        node->reg = i;

        nodes[i] = node;
        if(i)
        {
            node->next = graph;
            graph = node;
            continue;
        }
        
        graph = node;
    }

    register_makegraph_findlifetimes(func, nodes);
    register_makegraph_findinterference(func->nregisters, nodes);
    register_makegraph_findreliance(func, nodes);

    return nodes;
}