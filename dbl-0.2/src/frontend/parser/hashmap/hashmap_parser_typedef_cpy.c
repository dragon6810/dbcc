#include <frontend/parser/hashmap/hashmap_parser.h>

#include <string.h>

#include <frontend/parser/parser.h>
#include <std/assert/assert.h>

void hashmap_parser_typedef_cpy(void* dst, void* src)
{
    assert(dst);
    assert(src);
    
    memcpy(dst, src, sizeof(parser_typedef_t));
    ((parser_typedef_t*)dst)->name = strdup(((parser_typedef_t*)dst)->name);
}