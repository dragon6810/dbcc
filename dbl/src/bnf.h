#ifndef _bnf_h
#define _bnf_h

#include <stdbool.h>

#include "darr.h"

typedef struct bnf_spec_edge_s
{
    int min, max;
    struct bnf_spec_node_s* node;
} bnf_spec_edge_t;

typedef struct bnf_spec_node_s
{
    char* dyn;        // If not null, node is a dynamic symbol
    char* def;        // If not null, node is a default
    char* symbol;     // Symbol name
    darr_t children;  // Array of darr_ts of bnf_spec_edge_t

    // Loading optimizations
    unsigned long int itkn;
} bnf_spec_node_t;

typedef struct bnf_spec_tree_s
{
    darr_t nodes;
} bnf_spec_tree_t;

extern bnf_spec_tree_t bnf_spec;

bool bnf_loadspec(char* filepath);

#endif
