#include <hashmap/string/string.h>

#include <string.h>

#include <assert/assert.h>

void hashmap_string_copy(void* dst, void* src)
{
    char *strsrc;

    assert(dst);
    assert(src);

    strsrc = *((char**)src);

    *((char**)dst) = strdup(strsrc);
}
