#include <middleend/ir/ir.h>

#include <stdio.h>

#include <std/assert/assert.h>

void ir_print_printvalue(ir_instruction_param_value_t* val)
{
    assert(val);

    switch(val->type)
    {
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_VOID:
        printf("void");
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_I8:
        printf("%hhd", val->i8);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_U8:
        printf("%hhu", val->u8);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_I16:
        printf("%hd", val->i16);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_U16:
        printf("%hu", val->u16);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_I32:
        printf("%d", val->i32);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_U32:
        printf("%u", val->u32);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_I64:
        printf("%ld", val->i64);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_U64:
        printf("%lu", val->u64);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_F32:
        printf("%f", val->f32);
        break;
    case IR_INSTRUCTION_PARAM_VALUE_TYPE_F64:
        printf("%lf", val->f64);
        break;
    default:
        break;
    }
}

void ir_print_printinstruction(ir_instruction_t* instruction)
{
    const char* opcode_names[] = 
    {
        "SALLOC", "DALLOC", "ADD", "SUB", "INC", "DEC", 
        "AND", "OR", "XOR", "SHL", "SHR", "MOV", "MUL", 
        "DIV", "CMP", "JMP", "CAL", "RET", "JE", "JG", 
        "JL", "JLE", "JGE"
    };

    int i;

    ir_instruction_param_t* param;
    unsigned char type;

    printf("%s ", opcode_names[instruction->opcode]);
    
    for(i=0; i<IR_MAXPARAMS; i++) 
    {
        param = &instruction->params[i];
        type = param->value.flags & 0x7;
        
        if (i > 0 && type != 0)
            printf(", ");
            
        switch (type) 
        {
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEINVALID: /* Empty param */
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEREGISTER: /* Register */
            printf("%%%lu", param->value.u64);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEADDRESS: /* Memory address */
            printf("[$%lu]", param->value.u64);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEINDIRECT: /* Indirect address */
            printf("[%%%lu]", param->value.u64);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGECONSTANT: /* Constant */
            printf("$");
            ir_print_printvalue(&param->value);
            break;
        }
    }
    
    printf("\n");
}

void ir_print(ir_translationunit_t* ir)
{
    ir_instruction_t* in;
    
    printf("--- IR Code ---\n");
    
    for(in=ir->instructions; in; in=in->next)
        ir_print_printinstruction(in);
        
    printf("---------------\n");
}