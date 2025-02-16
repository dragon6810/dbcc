#include <frontend/lexer/lexer.h>

#include <string.h>

#include <assert/assert.h>

bool hashmap_lexer_define_cmp(void* a, void* b)
{
    assert(a);
    assert(b);

    return !memcmp(a, b, sizeof(lexer_define_t));
}