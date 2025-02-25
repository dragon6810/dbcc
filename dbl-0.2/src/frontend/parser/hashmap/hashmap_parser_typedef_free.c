#include <frontend/parser/hashmap/hashmap_parser.h>

#include <string.h>

#include <frontend/parser/parser.h>
#include <std/assert/assert.h>

void hashmap_parser_typedef_free(void* p)
{
    assert(p);
    
    free(((parser_typedef_t*)p)->name);
}