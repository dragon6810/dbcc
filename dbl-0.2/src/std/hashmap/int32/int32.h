#ifndef _hashmap_int32_h
#define _hashmap_int32_h

#include <stdbool.h>
#include <stdint.h>

unsigned long int hashmap_int32_hash(void* data);
bool hashmap_int32_cmp(void* a, void* b);

#endif
