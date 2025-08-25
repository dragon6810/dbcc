#include <backend/arch/arm/arm.h>

#include <std/assert/assert.h>

#include <string.h>

void arm_makeexecutable(ir_translationunit_t* ir, executable_t* exec)
{
    const uint8_t placeholder_exit[] = 
    {
        0x10, 0x20, 0x80, 0xd2,    // mov x16, #1
        0x10, 0x00, 0x80, 0xd2,    // mov x0, #0
        0x90, 0x00, 0x00, 0xd4,    // svc #0x80
    };

    assert(ir);
    assert(exec);

    // just exit for now
    exec->textsize = sizeof(placeholder_exit);
    exec->text = malloc(exec->textsize);
    memcpy(exec->text, placeholder_exit, exec->textsize);
    exec->entry = 0;
}