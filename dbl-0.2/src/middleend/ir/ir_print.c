#include <middleend/ir/ir.h>

#include <stdio.h>

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
            printf("%%%d", param->value.i32);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEADDRESS: /* Memory address */
            printf("[$%d]", param->value.i32);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGEINDIRECT: /* Indirect address */
            printf("[%%%d]", param->value.i32);
            break;
        case IR_INSTRUCTION_PARAM_VALUE_FLAGS_STORAGECONSTANT: /* Constant */
            printf("$%d", param->value.i32);
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