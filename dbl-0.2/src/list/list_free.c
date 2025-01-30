#include <list/list.h>

#include <assert/assert.h>
#include <stdlib.h>
#include <string.h>

void list_free(list_t* list)
{
    assert(list);
    assert(list->data);

    free(list->data);
    memset(list, 0, sizeof(list_t));
}