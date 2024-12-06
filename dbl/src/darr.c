#include "darr.h"

#include <string.h>

void darr_init(darr_t* arr, size_t itemsize)
{
    arr->itemsize = itemsize;
    arr->data = NULL;
    arr->len = arr->realsize = 0;
} 

void darr_push(darr_t* arr, void* item)
{
    if(!arr->data)
    {
        arr->data = malloc(arr->itemsize);
        (void) memcpy(arr->data, item, arr->itemsize);
        arr->len = arr->realsize = 1;
        return;
    }

    if(arr->len < arr->realsize)
    {
        (void) memcpy(((char*) arr->data) + arr->itemsize * arr->len, item, arr->itemsize);
        arr->len++;
        return;
    }

    arr->realsize <<= 1;
    arr->data = realloc(arr->data, arr->realsize * arr->itemsize);
    (void) memcpy(((char*) arr->data) + arr->itemsize * arr->len, item, arr->itemsize);
    arr->len++;
}
