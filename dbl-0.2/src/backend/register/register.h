#ifndef _REGISTER_H
#define _REGISTER_H

#include <middleend/ir/ir.h>

typedef struct register_edge_s
{
    struct register_node_s *node;
    struct register_edge_s *next;
} register_edge_t;

typedef struct register_node_s
{
    int reg;
    int color; // machine register
    int life[2];
    register_edge_t *interfere;
    register_edge_t *rely;

    struct register_node_s *next;
} register_node_t;

// returns n spills
int register_colorgraph(int nreg, register_node_t** graph, int ncol);
register_node_t** register_makegraph(ir_definition_function_t* func);

#endif