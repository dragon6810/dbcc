#ifndef _bnf_h
#define _bnf_h

#include <stdbool.h>

#include "darr.h"

typedef struct bnf_spec_edge_s
{
    bool optional;
    int min, max;
    char* name;
    struct bnf_spec_node_s* node;
} bnf_spec_edge_s;

typedef struct bnf_spec_node_s
{
    char* def;        // If not null, child is a default
    char* symbol;     // Symbol name
    darr_t children;  // Array of bnf_spec_edge_t*
} bnf_spec_node_t;

typedef struct bnf_spec_tree_s
{
    darr_t nodes;
} bnf_spec_tree_t;

extern bnf_spec_tree_t spec;

bool bnf_loadspec(char* filepath);

#endif
