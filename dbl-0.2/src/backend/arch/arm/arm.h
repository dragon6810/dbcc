#ifndef _arm_h
#define _arm_h

#include <backend/backend.h>
#include <middleend/ir/ir.h>

void arm_makeexecutable(ir_translationunit_t* ir, executable_t* exec);
char* arm_genasm(ir_translationunit_t* ir);

#endif