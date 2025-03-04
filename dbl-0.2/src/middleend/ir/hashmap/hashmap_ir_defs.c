#include <middleend/ir/ir.h>

#include <middleend/ir/hashmap/hashmap_ir.h>

HASHMAP_TYPE_DEF
(
    hashmap_string_hash,
    hashmap_string_cmp,
    hashmap_string_free,
    hashmap_ir_label_free,
    hashmap_string_copy,
    hashmap_ir_label_cpy,
    string_ir_label
)