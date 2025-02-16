#include <std/list/list.h>

#include <stdlib.h>
#include <string.h>

#include <assert/assert.h>

void list_removerange(list_template_t* list, unsigned long int start, unsigned long int end)
{
    void *firstchunk, *lastchunk, *lastcopy;

    assert(list);
    assert(start < list->size);
    assert(end <= list->size);
    assert(start < end);

    firstchunk = list->data;
    lastchunk = list->data + end * list->elsize;
    lastcopy = malloc((list->size - end) * list->elsize);
    memcpy(lastcopy, lastchunk, (list->size - end) * list->elsize);

    list->size -= end - start;
    if(list->buffsize >> 1 >= list->size * list->elsize)
    {
        while(list->buffsize >> 1 >= list->size * list->elsize)
            list->buffsize >>= 1;

        free(list->data);
        list->data = malloc(list->buffsize);
    }

    memcpy(list->data, firstchunk, start * list->elsize);
    memcpy(list->data + start * list->elsize, lastcopy, (list->size - start) * list->elsize);

    free(lastcopy);
}