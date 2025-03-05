#ifndef _ir_h
#define _ir_h

#include <std/hashmap/hashmap.h>
#include <std/hashmap/hashmap_types.h>
#include <std/list/list.h>

#define IR_MAXPARAMS 4

#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEBITS 0x7
#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEINVALID 0
#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEREGISTER 1
#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEADDRESS 2
#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEINDIRECT 3
#define IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGECONSTANT 4

typedef struct ir_translationunit_s ir_translationunit_t;
typedef struct ir_instruction_s ir_instruction_t;
typedef union ir_instruction_param_u ir_instruction_param_t;
typedef struct ir_instruction_param_value_s ir_instruction_param_value_t;
typedef struct ir_label_s ir_label_t;

typedef enum
{
    /* Metadata */
    IR_INSTRUCTIONTYPE_SALLOC, /* stack alloc: ident name, type type */
    IR_INSTRUCTIONTYPE_DALLOC, /* data alloc: ident name, type type, value initval */

    /* Instructions */
    IR_INSTRUCTIONTYPE_ADD,
    IR_INSTRUCTIONTYPE_SUB,
    IR_INSTRUCTIONTYPE_INC,
    IR_INSTRUCTIONTYPE_DEC,
    IR_INSTRUCTIONTYPE_AND,
    IR_INSTRUCTIONTYPE_OR,
    IR_INSTRUCTIONTYPE_XOR,
    IR_INSTRUCTIONTYPE_SHL,
    IR_INSTRUCTIONTYPE_SHR,
    IR_INSTRUCTIONTYPE_MOV,
    IR_INSTRUCTIONTYPE_MUL,
    IR_INSTRUCTIONTYPE_DIV,
    IR_INSTRUCTIONTYPE_CMP,
    IR_INSTRUCTIONTYPE_JMP,
    IR_INSTRUCTIONTYPE_CAL,
    IR_INSTRUCTIONTYPE_RET,
    IR_INSTRUCTIONTYPE_JE,
    IR_INSTRUCTIONTYPE_JG,
    IR_INSTRUCTIONTYPE_JL,
    IR_INSTRUCTIONTYPE_JLE,
    IR_INSTRUCTIONTYPE_JGE,
} ir_instructiontype_e;

typedef enum
{
    IR_INSTRUCTION_PARAM_VALUE_TYPE_VOID,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_I8,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_U8,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_I16,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_U16,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_I32,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_U32,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_I64,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_U64,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_F32,
    IR_INSTRUCTION_PARAM_VALUE_TYPE_F64,
} ir_instruction_param_value_type_e;

LIST_TYPE(ir_instruction_t, list_ir_instruction)
HASHMAP_TYPE_DECL(char*, ir_label_t, string_ir_label)

struct ir_translationunit_s
{
    hashmap_string_ir_label_t labels;
    ir_instruction_t *instructions;
};

struct ir_label_s
{
    char *name;
    ir_instruction_t *instruction;
};

struct ir_instruction_param_value_s
{
    unsigned char flags;
    ir_instruction_param_value_type_e type;
    union
    {
        char i8;
        unsigned char u8;
        short i16;
        unsigned short u16;
        int i32;
        unsigned int u32;
        long int i64;
        unsigned long int u64;
        float f32;
        double f64;
    };
};

union ir_instruction_param_u
{
    ir_instruction_param_value_t value;
};

struct ir_instruction_s
{
    ir_instructiontype_e opcode;
    ir_instruction_param_t params[IR_MAXPARAMS];
    
    ir_instruction_t *next, *last;
};

void ir_print(ir_translationunit_t* ir);

#endif