#include <middleend/ir/ir.h>

#include <stdio.h>

#include <std/assert/assert.h>

void ir_print_printfuncdeclaration(ir_translationunit_t* ir, ir_declaration_function_t* decl)
{
    assert(ir);
    assert(decl);

    printf("function i32 %s ( )\n", decl->name);
}

void ir_print_printinstruction(ir_translationunit_t* ir, ir_instruction_t* in)
{
    assert(ir);
    assert(in);

    switch (in->opcode)
    {
    case IR_INSTRUCTIONTYPE_LOADCONST:
        printf("%%%llu = %d\n", in->loadconst.reg.reg, in->loadconst.val.val);

        break;
    case IR_INSTRUCTIONTYPE_RETURN:
        printf("return %%%llu\n", in->ret.reg.reg);
        break;
    case IR_INSTRUCTIONTYPE_ADD:
        printf("%%%llu = %%%llu + %%%llu\n", in->add.dst.reg, in->add.a.reg, in->add.b.reg);
        break;
    default:
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
        printf("    ");
        ir_print_printinstruction(ir, in);
    }
}

void ir_print_printfuncdefinition(ir_translationunit_t* ir, ir_definition_function_t* def)
{
    ir_print_printfuncdeclaration(ir, &def->decl);
    printf("{\n");
    ir_print_printfuncbody(ir, def);
    printf("}\n");
}

void ir_print(ir_translationunit_t* ir)
{
    int i;
    
    assert(ir);

    printf("======== IR Code ========\n\n");
    
    for(i=0; i<ir->body.size; i++)
    {
        if(ir->body.data[i].isdef)
            ir_print_printfuncdefinition(ir, &ir->body.data[i].def);
        else   
            ir_print_printfuncdeclaration(ir, &ir->body.data[i].decl);

        printf("\n");
    }
        
    printf("=========================\n");
}