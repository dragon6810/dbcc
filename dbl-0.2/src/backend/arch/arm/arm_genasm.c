#include <backend/arch/arm/arm.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <std/assert/assert.h>

#define N_REG_WORD 13

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

static void arm_genasm_genregmap(ir_definition_function_t* func, int* outmap)
{
    int i, icmd, j, k;
    ir_instruction_t *cmd;

    int nreg;
    int regpeak; // the maximum number of registers used at once
    int regstarts[func->nregisters], regends[func->nregisters];
    int nactive, active[func->nregisters];
    int clobbered[3];
    int revmap[N_REG_WORD]; // reverse map for efficiency

    for(i=0; i<func->nregisters; i++)
    {
        regstarts[i] = regends[i] = -1;
    }

    for(cmd=func->instructions, icmd=0; cmd; cmd=cmd->next, icmd++)
    {
        for(i=0; i<3; i++)
            clobbered[i] = -1;

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
            regends[clobbered[i]] = icmd + 1;
        }
    }

    regpeak = nreg = 0;
    for(i=0; i<icmd; i++)
    {
        for(j=0; j<func->nregisters; j++)
        {
            if(regstarts[j] == i)
                nreg++;
            if(regends[j] == i)
                nreg--;
        }

        if(nreg > regpeak)
            regpeak = nreg;
    }

    assert(regpeak < N_REG_WORD && "TODO: register spilling.\n");

    for(i=0; i<func->nregisters; i++)
        outmap[i] = -1;
    for(i=0; i<N_REG_WORD; i++)
        revmap[i] = -1;

    for(i=0; i<func->nregisters; i++)
    {
        nactive = 0;
        for(j=0; j<func->nregisters; j++)
        {
            if(i == j)
                continue;

            if(regstarts[i] < regstarts[j])
                continue;
            if(regstarts[i] >= regends[j])
                continue;

            active[nactive++] = j;
        }

        for(j=0; j<N_REG_WORD; j++)
        {
            for(k=0; k<nactive; k++)
            {
                if(outmap[active[k]] == j)
                    break;
            }

            // j is taken!
            if(k < nactive)
                continue;

            break;
        }

        assert(j < N_REG_WORD);

        outmap[i] = j;
    }

    printf("peak registers: %d.\n", regpeak);
    for(i=0; i<func->nregisters; i++)
    {
        printf("%%%d -> %s\n", i, name_reg_word[outmap[i]]);
    }
}

static void arm_genasm_func(FILE* ptr, ir_definition_function_t* func, bool main)
{
    ir_instruction_t *cmd;
    int regmap[func->nregisters];

    if(!main)
        return;

    arm_genasm_genregmap(func, regmap);

    fprintf(ptr, "_start:\n");

    for(cmd=func->instructions; cmd; cmd=cmd->next)
    {
        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            fprintf(ptr, "  mov %s, #%d\n", name_reg_word[regmap[cmd->loadconst.reg.reg]], cmd->loadconst.val.val);
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            fprintf(ptr, "  sxtw X0, %s\n", name_reg_word[regmap[cmd->ret.reg.reg]]);
            fprintf(ptr, "  mov X16, #1\n");
            fprintf(ptr, "  svc #0x80\n");
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            fprintf(ptr, "  add %s, %s, %s\n", 
                name_reg_word[regmap[cmd->add.dst.reg]], 
                name_reg_word[regmap[cmd->add.a.reg]], 
                name_reg_word[regmap[cmd->add.b.reg]]);
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            fprintf(ptr, "  mul %s, %s, %s\n", 
                name_reg_word[regmap[cmd->mult.dst.reg]], 
                name_reg_word[regmap[cmd->mult.a.reg]], 
                name_reg_word[regmap[cmd->mult.b.reg]]);
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