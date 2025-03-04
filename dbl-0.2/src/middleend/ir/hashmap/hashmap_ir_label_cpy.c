#include <middleend/ir/hashmap/hashmap_ir.h>

#include <string.h>

#include <middleend/ir/ir.h>
#include <std/assert/assert.h>

void hashmap_ir_label_cpy(void* dst, void* src)
{
    assert(dst);
    assert(src);

    memcpy(dst, src, sizeof(ir_label_t));

    ((ir_label_t*)dst)->name = strdup(((ir_label_t*)dst)->name);
}