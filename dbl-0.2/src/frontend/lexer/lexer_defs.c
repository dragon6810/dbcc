#include <frontend/lexer/lexer.h>

#include <frontend/lexer/hashmap/hashmap_lexer.h>

HASHMAP_TYPE_DEF
(
    hashmap_string_hash,
    hashmap_lexer_define_cmp,
    hashmap_string_free,
    hashmap_lexer_define_free,
    hashmap_string_copy,
    hashmap_lexer_define_cpy,
    string_lexer_define
)