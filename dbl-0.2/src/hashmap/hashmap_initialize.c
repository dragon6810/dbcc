#include <hashmap/hashmap.h>

#include <string.h>

#include <assert/assert.h>

void hashmap_initialize(hashmap_t* hashmap, unsigned long int (*hash)(void*), bool (*cmp)(void*, void*), unsigned long int nbuckets, unsigned long int keysize, unsigned long int valsize, void (*freekey)(void*), void (*freeval)(void*))
{
    assert(hashmap);
    assert(hash);

    memset(hashmap, 0, sizeof(hashmap_t));

    hashmap->nbuckets = nbuckets;
    hashmap->buckets = calloc(hashmap->nbuckets, sizeof(hashmap_bucket_t));
    hashmap->keysize = keysize;
    hashmap->valsize = valsize;
    hashmap->hash = hash;
    hashmap->cmp = cmp;
}
