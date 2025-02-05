#include <hashmap/hashmap.h>

#include <assert/assert.h>

void hashmap_initialize(hashmap_t* hashmap, unsigned long int (*hash)(void*), unsigned long int nbuckets)
{
    assert(hashmap);
    assert(hash);

    memset(hashmap, 0, sizeof(hashmap_t));
     
}
