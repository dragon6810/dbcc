#include <middleend/ir/ir.h>

#include <stddef.h>

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

HASHMAP_TYPE_DEF
(
    hashmap_string_hash,
    hashmap_string_cmp,
    hashmap_string_free,
    NULL,
    hashmap_string_copy,
    NULL,
    string_ir_declaration_p
)

HASHMAP_TYPE_DEF
(
    hashmap_string_hash,
    hashmap_string_cmp,
    hashmap_string_free,
    NULL,
    hashmap_string_copy,
    NULL,
    string_ir_definition_p
)