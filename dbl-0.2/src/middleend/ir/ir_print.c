#include <middleend/ir/ir.h>

#include <stdio.h>

#include <std/assert/assert.h>

void ir_print_printfuncdeclaration(ir_translationunit_t* ir, ir_declaration_function_t* decl)
{
    printf("function %s \n", decl->name);
}

void ir_print_printdeclaration(ir_translationunit_t* ir, ir_declaration_t* decl)
{
    assert(ir);
    assert(decl);

    printf("declare ");

    switch(decl->type)
    {
    case IR_DECLARATIONTYPE_VARIABLE:
        puts("IR_PRINT: TODO: Variable Declaration");

        break;
    case IR_DECLARATIONTYPE_FUNCTION:
        ir_print_printfuncdeclaration(ir, &decl->function);
        
        break;
    }
}

void ir_print(ir_translationunit_t* ir)
{
    int i;
    
    assert(ir);

    printf("--- IR Code ---\n");
    
    for(i=0; i<ir->body.size; i++)
    {
        if(ir->body.data[i].isdef)
            puts("IR_PRINT: TODO: Definition");
        
        ir_print_printdeclaration(ir, &ir->body.data[i].decl);
    }
        
    printf("---------------\n");
}