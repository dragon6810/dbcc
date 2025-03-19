#include <middleend/ir/ir.h>

#include <stdio.h>

#include <std/assert/assert.h>

void ir_print_printdeclaration(ir_translationunit_t* ir, ir_declaration_t* decl)
{

}

void ir_print(ir_translationunit_t* ir)
{
    int i;
    
    printf("--- IR Code ---\n");
    
    for(i=0; i<ir->body.size; i++)
    {
        if(ir->body.data[i].isdef)
            puts("IR_PRINT: TODO: Definition");
        
        ir_print_printdeclaration(ir, &ir->body.data[i].decl);
    }
        
    printf("---------------\n");
}