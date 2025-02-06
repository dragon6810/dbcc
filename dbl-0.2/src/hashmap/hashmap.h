#ifndef _hashmap_h
#define _hashmap_h

#include <stdbool.h>

/*
 * ================================
 *             MACROS
 * ================================
*/

#define HASHMAP_BUCKETS_SMALL  16
#define HASHMAP_BUCKETS_MEDIUM 64
#define HASHMAP_BUCKETS_LARGE  1024

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct hashmap_s hashmap_t;
typedef struct hashmap_bucket_s hashmap_bucket_t;
typedef struct hashmap_bucketitem_s hashmap_bucketitem_t;

struct hashmap_s
{
    hashmap_bucket_t *buckets;
    unsigned long int nbuckets;
    unsigned long int keysize, valsize;

    unsigned long int (*hash)(void*);
    bool (*cmp)(void*, void*);
    void (*freekey)(void*);
    void (*freeval)(void*);
    void (*copykey)(void*, void*);
    void (*copyval)(void*, void*);
};

struct hashmap_bucket_s
{
    hashmap_bucketitem_t *items;
};

struct hashmap_bucketitem_s
{
    hashmap_bucketitem_t *next;
    unsigned long int hash;
    void *key, *val;
};

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void hashmap_initialize(hashmap_t* hashmap, unsigned long int (*hash)(void*),  bool (*cmp)(void*, void*), unsigned long int nbuckets, unsigned long int keysize, unsigned long int valsize, void (*freekey)(void*), void (*freeval)(void*), void  (*copykey)(void*, void*), void  (*copyval)(void*, void*));
void* hashmap_fetch(hashmap_t* hashmap, void* key);
void* hashmap_set(hashmap_t* hashmap, void* key, void* val);
void hashmap_free(hashmap_t* hashmap);

#endif
