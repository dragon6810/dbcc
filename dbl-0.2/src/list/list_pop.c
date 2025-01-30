#include <list/list.h>

#include <assert/assert.h>
#include <string.h>
#include <stdlib.h>

void list_pop(list_t* list, void* element)
{
    assert(list->elsize);
    assert(list->size);

    if(element)
        memcpy(element, list->data + (list->size - 1) * list->elsize, list->elsize);

    list->size--;

    if((list->buffsize >> 1) < list->size * list->elsize)
        return;

    list->buffsize >>= 1;
    list->data = realloc(list->data, list->buffsize);
}
