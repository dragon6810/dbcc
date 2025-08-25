#include <backend/arch/arm/arm.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void arm_genasm_func(FILE* ptr, ir_definition_function_t* func, bool main)
{
    ir_instruction_t *cmd;

    // put everything into X0 for now

    if(!main)
        return;

    fprintf(ptr, "_start:\n");

    for(cmd=func->instructions; cmd; cmd=cmd->next)
    {
        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            fprintf(ptr, "  mov W%llu, #%d\n", cmd->loadconst.reg.reg, cmd->loadconst.val.val);
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            fprintf(ptr, "  sxtw X0, W%llu\n", cmd->ret.reg.reg);
            fprintf(ptr, "  mov X16, #1\n");
            fprintf(ptr, "  svc #0x80\n");
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            fprintf(ptr, "  add W%llu, W%llu, W%llu\n", cmd->add.dst.reg, cmd->add.a.reg, cmd->add.b.reg);
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            fprintf(ptr, "  mul W%llu, W%llu, W%llu\n", cmd->mult.dst.reg, cmd->mult.a.reg, cmd->mult.b.reg);
            break;
        default:
            break;
        }
    }
}

char* arm_genasm(ir_translationunit_t* ir)
{
    int i;

    FILE *ptr;

    ptr = fopen("out.s", "w");

    fprintf(ptr, ".global _start\n");

    for(i=0; i<ir->body.size; i++)
    {
        if(ir->body.data[i].isdef && !strcmp(ir->body.data[i].def.decl.name, "@main"))
        {
            arm_genasm_func(ptr, &ir->body.data[i].def, true);
        }
    }

    fclose(ptr);

    return NULL;
}