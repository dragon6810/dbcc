#ifndef _ir_h
#define _ir_h

#include <stdint.h>

#include <std/hashmap/hashmap.h>
#include <std/hashmap/hashmap_types.h>
#include <std/list/list.h>

typedef struct ir_translationunit_s ir_translationunit_t;
typedef struct ir_declordef_s ir_declordef_t;
typedef struct ir_declaration_function_s ir_declaration_function_t;
typedef struct ir_definition_function_s ir_definition_function_t;
typedef struct ir_instruction_s ir_instruction_t;
typedef struct ir_instruction_loadconst_s ir_instruction_loadconst_t;
typedef struct ir_instruction_return_s ir_instruction_return_t;
typedef struct ir_instruction_add_s ir_instruction_add_t;
typedef struct ir_value_s ir_value_t;
typedef struct ir_value_register_s ir_value_register_t;
typedef struct ir_value_constant_s ir_value_constant_t;

typedef uint64_t ir_regindex_t;

typedef enum
{
    IR_DECLARATIONTYPE_VARIABLE,
    IR_DECLARATIONTYPE_FUNCTION,
} ir_declarationtype_e;

typedef enum
{
    IR_INSTRUCTIONTYPE_LOADCONST,
    IR_INSTRUCTIONTYPE_RETURN,
    IR_INSTRUCTIONTYPE_ADD,
} ir_instructiontype_e;

LIST_TYPE(ir_value_t, list_ir_value)
LIST_TYPE(ir_declordef_t, list_ir_declordef)

struct ir_structinit_s
{
    list_ir_value_t values;
};

struct ir_translationunit_s
{
    list_ir_declordef_t body;
};

struct ir_value_constant_s
{
    int32_t val; // only i32 for now
};

struct ir_value_register_s
{
    ir_regindex_t reg;
};

struct ir_value_s
{
    ir_value_constant_t val; // no registers for now
};

struct ir_declaration_function_s
{
    // assume return type of i32 for now
    char *name;
};

struct ir_definition_function_s
{
    ir_declaration_function_t decl;

    ir_instruction_t *instructions;   /* first in a doubly linked list */
    ir_instruction_t *insttail;
    unsigned long long nregisters;
};

struct ir_declordef_s
{
    bool isdef;
    union
    {
        ir_declaration_function_t decl;
        ir_definition_function_t def;
    };
};

struct ir_instruction_loadconst_s
{
    ir_value_register_t reg;
    ir_value_constant_t val;
};

struct ir_instruction_return_s
{
    // only return registers for now
    ir_value_register_t reg;
};

struct ir_instruction_add_s
{
    ir_value_register_t dst;
    ir_value_register_t a;
    ir_value_register_t b;
};

struct ir_instruction_s
{
    ir_instructiontype_e opcode;
    union
    {
        ir_instruction_loadconst_t loadconst;
        ir_instruction_return_t ret;
        ir_instruction_add_t add;
    };
    
    ir_instruction_t *next, *last;
};

void ir_print(ir_translationunit_t* ir);

#endif