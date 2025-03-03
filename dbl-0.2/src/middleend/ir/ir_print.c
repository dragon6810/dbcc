#include <middleend/ir/ir.h>

#include <stdio.h>

void ir_print_printinstruction(ir_instruction_t* instruction)
{
    printf("todo: print instruction");
}

void ir_print(ir_translationunit_t* ir)
{
    ir_instruction_t* in;

    for(in=ir->instructions; in; in=in->next)
        ir_print_printinstruction(in);
}