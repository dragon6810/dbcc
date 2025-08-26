#include <std/hashmap/hashmap.h>

#include <stdint.h>

#include <std/assert/assert.h>

unsigned long int hashmap_int32_hash(void* data)
{
    assert(data);

    return *((uint32_t*) data);
}
