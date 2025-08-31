#include <backend/arch/arm/arm.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <backend/register/register.h>
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

static void arm_genasm_func(FILE* ptr, ir_definition_function_t* func, bool main)
{
    ir_instruction_t *cmd;
    register_node_t **graph;

    if(!main)
        return;

    graph = register_makegraph(func);
    register_colorgraph(func->nregisters, graph, N_REG_WORD);

    fprintf(ptr, "_start:\n");

    for(cmd=func->instructions; cmd; cmd=cmd->next)
    {
        switch(cmd->opcode)
        {
        case IR_INSTRUCTIONTYPE_LOADCONST:
            fprintf(ptr, "  mov %s, #%d\n", name_reg_word[graph[cmd->loadconst.reg.reg]->color], cmd->loadconst.val.val);
            break;
        case IR_INSTRUCTIONTYPE_RETURN:
            fprintf(ptr, "  sxtw X0, %s\n", name_reg_word[graph[cmd->ret.reg.reg]->color]);
            fprintf(ptr, "  mov X16, #1\n");
            fprintf(ptr, "  svc #0x80\n");
            break;
        case IR_INSTRUCTIONTYPE_ADD:
            fprintf(ptr, "  add %s, %s, %s\n", 
                name_reg_word[graph[cmd->add.dst.reg]->color], 
                name_reg_word[graph[cmd->add.a.reg]->color], 
                name_reg_word[graph[cmd->add.b.reg]->color]);
            break;
        case IR_INSTRUCTIONTYPE_MULT:
            fprintf(ptr, "  mul %s, %s, %s\n", 
                name_reg_word[graph[cmd->mult.dst.reg]->color], 
                name_reg_word[graph[cmd->mult.a.reg]->color], 
                name_reg_word[graph[cmd->mult.b.reg]->color]);
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