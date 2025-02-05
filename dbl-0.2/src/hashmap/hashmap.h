#ifndef _hashmap_h
#define _hashmap_h

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
    
    unsigned long int (*hash)(void*);
};

struct hashmap_bucket_s
{
    hashmap_bucketitem_t *items;
};

struct hashmap_bucketitem_s
{
    hashmap_bucketitem_t *next;
    void *payload;
};

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void hashmap_initialize(hashmap_t* hashmap, unsigned long int (*hash)(void*), unsigned long int nbuckets);

#endif
