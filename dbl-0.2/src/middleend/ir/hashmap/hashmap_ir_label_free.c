#include <middleend/ir/hashmap/hashmap_ir.h>

#include <middleend/ir/ir.h>
#include <std/assert/assert.h>

void hashmap_ir_label_free(void* p)
{
    assert(p);

    free(((ir_label_t*)p)->name);
}