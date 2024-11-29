#include "darr.h"

#include <string.h>

void darr_init(darr_t* arr, size_t itemsize)
{
    arr->itemsize = itemsize;
} 

void darr_push(darr_t* arr, void* item)
{
    if(!arr->data)
    {
        arr->data = malloc(arr->itemsize);
        memcpy(arr->data, item, arr->itemsize);
        arr->len = arr->realsize = 1;
        return;
    }

    if(arr->len < arr->realsize)
    {
        memcpy(arr->data + arr->itemsize * arr->len, item, arr->itemsize);
        arr->len++;
        return;
    }

    arr->realsize <<= 1;
    arr->data = realloc(arr->data, arr->realsize);
    memcpy(arr->data + arr->itemsize * arr->len, item, arr->itemsize);
    arr->len++;
}
