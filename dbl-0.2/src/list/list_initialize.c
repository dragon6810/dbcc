#include <list/list.h>

#include <assert/assert.h>
#include <string.h>

void list_initialize(list_t* list, unsigned long int elsize)
{
    assert(elsize);

    memset(list, 0, sizeof(list_t));
    list->elsize = elsize;
}
