#include <list/list.h>

#include <assert/assert.h>
#include <string.h>
#include <stdlib.h>

void list_copy(list_t* new, list_t* old)
{
    assert(new);
    assert(old);

    memcpy(new, old, sizeof(list_t));
    new->data = malloc(new->buffsize);
    memcpy(new->data, old->data, new->buffsize);
}