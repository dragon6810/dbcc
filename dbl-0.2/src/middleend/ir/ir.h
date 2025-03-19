#ifndef _ir_h
#define _ir_h

#include <stdint.h>

#include <std/hashmap/hashmap.h>
#include <std/hashmap/hashmap_types.h>
#include <std/list/list.h>

typedef struct ir_translationunit_s ir_translationunit_t;
typedef struct ir_declordef_s ir_declordef_t;
typedef struct ir_declaration_s ir_declaration_t;
typedef struct ir_declaration_variable_s ir_declaration_variable_t;
typedef struct ir_declaration_function_s ir_declaration_function_t;
typedef struct ir_definition_variable_s ir_definition_variable_t;
typedef struct ir_definition_function_s ir_definition_function_t;
typedef struct ir_type_s ir_type_t;
typedef struct ir_instruction_s ir_instruction_t;
typedef struct ir_value_s ir_value_t;
typedef struct ir_label_s ir_label_t;
typedef struct ir_value_constant_s ir_value_constant_t;
typedef struct ir_value_register_s ir_value_register_t;
typedef struct ir_value_directaddr_s ir_value_directaddr_t;
typedef struct ir_value_indirectaddr_s ir_value_indirectaddr_t;
typedef struct ir_value_indirectsum_s ir_value_indirectsum_t;
typedef struct ir_value_offsetindirect_s ir_value_offsetindirect_t;
typedef struct ir_value_offsetvariable_s ir_value_offsetvariable_t;
typedef struct ir_structinit_s ir_structinit_t;

typedef unsigned long int ir_regindex_t;
typedef unsigned long int ir_address_t;

typedef enum
{
    IR_DECLARATIONTYPE_VARIABLE,
    IR_DECLARATIONTYPE_FUNCTION,
} ir_declarationtype_e;

typedef enum
{
    IR_INSTRUCTIONTYPE_ADD,
    IR_INSTRUCTIONTYPE_SUB,
    IR_INSTRUCTIONTYPE_MUL,
    IR_INSTRUCTIONTYPE_DIV,
    IR_INSTRUCTIONTYPE_CAST,
    IR_INSTRUCTIONTYPE_LOAD,
    IR_INSTRUCTIONTYPE_CMP,
    IR_INSTRUCTIONTYPE_JMP,
    IR_INSTRUCTIONTYPE_JE,
    IR_INSTRUCTIONTYPE_JNE,
    IR_INSTRUCTIONTYPE_JL,
    IR_INSTRUCTIONTYPE_JG,
    IR_INSTRUCTIONTYPE_JLE,
    IR_INSTRUCTIONTYPE_JGE,
    IR_INSTRUCTIONTYPE_CALL,
    IR_INSTRUCTIONTYPE_RETURN,
    IR_INSTRUCTIONTYPE_SALLOC,
} ir_instructiontype_e;

typedef enum
{
    /* primitives */
    IR_TYPE_VOID,
    IR_TYPE_I8,
    IR_TYPE_U8,
    IR_TYPE_I16,
    IR_TYPE_U16,
    IR_TYPE_I32,
    IR_TYPE_U32,
    IR_TYPE_I64,
    IR_TYPE_U64,
    IR_TYPE_F32,
    IR_TYPE_F64,
    IR_TYPE_STR,

    /* custom types */
    IR_TYPE_STRUCT,
    IR_TYPE_ENUM,
    IR_TYPE_UNION,
    IR_TYPE_TYPEDEF,
} ir_type_e;

typedef enum
{
    IR_VALUE_TYPE_CONSTANT,
    IR_VALUE_TYPE_REGISTER,
    IR_VALUE_TYPE_DIRECTADDRESS,
    IR_VALUE_TYPE_INDIRECTADDRESS,
    IR_VALUE_TYPE_INDIRECTSUMADDRESS,
    IR_VALUE_TYPE_OFFSETINDIRECTADDRESS,
} ir_value_type_e;

HASHMAP_TYPE_DECL(char*, ir_label_t, string_ir_label)
LIST_TYPE(ir_declaration_t, list_ir_declaration)
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

struct ir_type_s
{
    ir_type_e type;
    char *name;
};

struct ir_declaration_variable_s
{
    ir_type_t type;
    char *name;       /* globals should start with '#', stack with '$' */
};

struct ir_value_constant_s
{
    union
    {
        int8_t          i8;
        uint8_t         u8;
        int16_t         i16;
        uint16_t        u16;
        int32_t         i32;
        uint32_t        u32;
        int64_t         i64;
        uint64_t        u64;
        float           f32;
        double          f64;
        char           *str;
        
        ir_structinit_t struc;
    };
};

struct ir_value_register_s
{
    ir_regindex_t reg;
};

struct ir_value_directaddr_s
{
    ir_address_t addr;
};

struct ir_value_indirectaddr_s
{
    ir_regindex_t reg;
};

struct ir_value_indirectsum_s
{
    ir_regindex_t reg[2];
};

struct ir_value_offsetindirect_s
{
    ir_regindex_t base;
    int64_t index, scale;
    int64_t offset;
};

struct ir_value_offsetvariable_s
{
    char *base;
    int64_t index, scale;
    int64_t offset;
};

struct ir_value_s
{
    ir_value_type_e valtype;
    ir_type_t type;
};

LIST_TYPE(ir_declaration_variable_t, list_ir_declaration_variable)

struct ir_declaration_function_s
{
    ir_type_t type;
    char *name;

    list_ir_declaration_variable_t parameters;
    bool variadic;                             /* in c, is there a '...' in the param list? */
};

struct ir_definition_variable_s
{
    ir_declaration_variable_t decl;

    ir_value_t value;               /* if 'void', default */
};

struct ir_declaration_s
{
    ir_declarationtype_e type;
    union
    {
        ir_declaration_variable_t variable;
        ir_declaration_function_t function;
    };
};

struct ir_definition_function_s
{
    ir_declaration_function_t decl;

    ir_instruction_t *instructions;   /* first in a doubly linked list */
    hashmap_string_ir_label_t labels;
};

struct ir_declordef_s
{
    bool isdef;
    union
    {
        ir_declaration_t decl;
    };
};

struct ir_label_s
{
    char *name;
    ir_instruction_t *instruction;
};

union ir_instruction_param_u
{
    ir_value_t value;
};

struct ir_instruction_s
{
    ir_instructiontype_e opcode;
    
    ir_instruction_t *next, *last;
};

void ir_print(ir_translationunit_t* ir);

#endif