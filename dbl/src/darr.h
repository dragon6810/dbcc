#ifndef _darr_h
#define _darr_h

#include <stdlib.h>

typedef struct darr_s
{
    size_t itemsize;
    void* data;
    unsigned long int len;
    unsigned long int realsize;
} darr_t;

void darr_init(darr_t* arr, size_t itemsize); 
void darr_push(darr_t* arr, void* item);

#endif
