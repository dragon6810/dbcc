#include  <frontend/lexer/lexer.h>

#include <string.h>

#include <std/assert/assert.h>

void hashmap_lexer_define_cpy(void* dst, void* src)
{
    assert(dst);
    assert(src);
    
    memcpy(dst, src, sizeof(lexer_define_t));
}