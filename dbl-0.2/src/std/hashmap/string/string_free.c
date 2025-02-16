#include <std/hashmap/string/string.h>

#include <assert/assert.h>

void hashmap_string_free(void* data)
{
    char *str;

    assert(data);

    str = *((char**)data);
    assert(str);

    free(str);
}
