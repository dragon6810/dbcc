#include "darr.h"

#include <string.h>

#include "error.h"

void darr_init(darr_t* arr, size_t itemsize)
{
    arr->itemsize = itemsize;
    arr->data = NULL;
    arr->len = arr->realsize = 0;
} 

void darr_push(darr_t* arr, void* item)
{
    if(!arr->itemsize)
    {
        Error("darr_push: itemsize is 0, have you initialized the array?.\n");
    }

    if(!arr->data)
    {
        arr->data = malloc(arr->itemsize);
        memcpy(arr->data, item, arr->itemsize);
        arr->len = arr->realsize = 1;
        return;
    }

    if(arr->len < arr->realsize)
    {
        memcpy(((char*) arr->data) + arr->itemsize * arr->len, item, arr->itemsize);
        arr->len++;
        return;
    }

    arr->realsize <<= 1;
    arr->data = realloc(arr->data, arr->realsize * arr->itemsize);
    memcpy(((char*) arr->data) + arr->itemsize * arr->len, item, arr->itemsize);
    arr->len++;
}
