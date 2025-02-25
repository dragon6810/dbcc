#include <frontend/parser/parser.h>

#include <frontend/parser/hashmap/hashmap_parser.h>

HASHMAP_TYPE_DEF
(
    hashmap_string_hash,
    hashmap_string_cmp,
    hashmap_string_free,
    hashmap_parser_typedef_free,
    hashmap_string_copy,
    hashmap_parser_typedef_cpy,
    string_parser_typedef
)