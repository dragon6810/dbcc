#include <middleend/ir/ir.h>

#include <stdio.h>

#include <std/assert/assert.h>

void ir_print_printfuncparameter(ir_translationunit_t* ir, ir_declaration_variable_t* decl)
{
    assert(ir);
    assert(decl);

    printf("%s %s", decl->type.name, decl->name);
}

void ir_print_printfuncdeclaration(ir_translationunit_t* ir, ir_declaration_function_t* decl)
{
    int i;

    assert(ir);
    assert(decl);

    printf("function %s %s(", decl->type.name, decl->name);

    for(i=0; i<decl->parameters.size; i++)
    {
        ir_print_printfuncparameter(ir, &decl->parameters.data[i]);
        if(i < decl->parameters.size-1)
        printf(", ");
    }

    printf(")\n");
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

void ir_print_printfuncbody(ir_translationunit_t* ir, ir_definition_function_t* def)
{
    ir_instruction_t *in;

    assert(ir);
    assert(def);

    if(!def->instructions)
        return;

    for(in=def->instructions; in; in=in->next)
    {
        printf("    instruction\n");
    }
}

void ir_print_printfuncdefinition(ir_translationunit_t* ir, ir_definition_function_t* def)
{
    ir_print_printfuncdeclaration(ir, &def->decl);
    printf("{\n");
    ir_print_printfuncbody(ir, def);
    printf("}\n");
}

void ir_print_printdefinition(ir_translationunit_t* ir, ir_definition_t* def)
{
    assert(ir);
    assert(def);

    printf("define ");

    switch(def->type)
    {
    case IR_DECLARATIONTYPE_VARIABLE:
        puts("IR_PRINT: TODO: Variable Declaration");

        break;
    case IR_DECLARATIONTYPE_FUNCTION:
        ir_print_printfuncdefinition(ir, &def->function);
        
        break;
    }
}

void ir_print(ir_translationunit_t* ir)
{
    int i;
    
    assert(ir);

    printf("======== IR Code ========\n\n");
    
    for(i=0; i<ir->body.size; i++)
    {
        if(ir->body.data[i].isdef)
            ir_print_printdefinition(ir, &ir->body.data[i].def);
        else   
            ir_print_printdeclaration(ir, &ir->body.data[i].decl);

        printf("\n");
    }
        
    printf("=========================\n");
}