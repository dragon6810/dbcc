#include <backend/arch/arm/arm.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <std/assert/assert.h>

#define N_REG_WORD 13

#if 0
static const char* name_reg_word[N_REG_WORD] =
{
    "W0",
    "W1",
    "W2",
    "W3",
    "W4",
    "W5",
    "W6",
    "W7",
    "W8",
    "W9",
    "W10",
    "W11",
    "W12",
};
#endif

static void arm_genasm_func(FILE* ptr, ir_definition_function_t* func, bool main)
{
    const int nullrange = -1;

    int i, icmd;
    ir_instruction_t *cmd;

    // int regpeak; // the maximum number of registers used at once
    int regstarts[func->nregisters], regends[func->nregisters];
    int clobbered[3];

    if(!main)
        return;

    for(i=0; i<func->nregisters; i++)
    {
        regstarts[i] = regends[i] = -1;
    }

    for(cmd=func->instructions, icmd=0; cmd; cmd=cmd->next, icmd++)
    {
        for(i=0; i<3; i++)
            clobbered[i] = nullrange;

        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            clobbered[0] = cmd->loadconst.reg.reg;
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            clobbered[0] = cmd->ret.reg.reg;
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            clobbered[0] = cmd->add.dst.reg;
            clobbered[1] = cmd->add.a.reg;
            clobbered[2] = cmd->add.b.reg;
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            clobbered[0] = cmd->mult.dst.reg;
            clobbered[1] = cmd->mult.a.reg;
            clobbered[2] = cmd->mult.b.reg;
            break;
        }

        for(i=0; i<3; i++)
        {
            if(clobbered[i] < 0)
                break;
                
            if(regstarts[clobbered[i]] < 0)
                regstarts[clobbered[i]] = icmd;
            regends[clobbered[i]] = icmd;
        }
    }

    for(i=0; i<func->nregisters; i++)
    {
        printf("%%%d range: %d - %d.\n", i, regstarts[i], regends[i]);
    }

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