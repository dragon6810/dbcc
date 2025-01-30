#include "darr.h"

#include <string.h>
#include <stdio.h>

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
    printf("realsize, len: %d, %d.\n", arr->realsize, arr->len);
    arr->data = realloc(arr->data, arr->realsize * arr->itemsize);
    memcpy(((char*) arr->data) + arr->itemsize * arr->len, item, arr->itemsize);
    arr->len++;
}

darr_t darr_copy(darr_t* arr)
{
    darr_t newarr;

    newarr.itemsize = arr->itemsize;
    newarr.len = arr->len;
    newarr.realsize = arr->realsize;
    if(arr->realsize)
        newarr.data = malloc(arr->realsize * arr->itemsize);
    else
        newarr.data = 0;
    memcpy(newarr.data, arr->data, arr->realsize * arr->itemsize);

    return newarr;
}