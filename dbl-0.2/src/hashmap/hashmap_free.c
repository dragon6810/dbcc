#include <hashmap/hashmap.h>

#include <string.h>

#include <assert/assert.h>

void hashmap_free(hashmap_t* hashmap)
{
    int i;
    hashmap_bucket_t *curbucket;
    hashmap_bucketitem_t *curitem, *nextitem;

    assert(hashmap);
    assert(hashmap->buckets);
    assert(hashmap->nbuckets);

    for(i=0, curbucket=hashmap->buckets; i<hashmap->nbuckets; i++, curbucket++)
    {
        if(!curbucket->items)
            continue;

        for(curitem=nextitem=curbucket->items; nextitem; curitem=nextitem)
        {
            nextitem = curitem->next;

            if(hashmap->freekey)
                hashmap->freekey(curitem->key);
            if(hashmap->freeval)
                hashmap->freeval(curitem->val);

            free(curitem->key);
            free(curitem->val);
            free(curitem);
        }
    }

    free(hashmap->buckets);

    memset(hashmap, 0, sizeof(hashmap_t));
}
