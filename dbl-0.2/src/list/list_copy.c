#include <list/list.h>

#include <assert/assert.h>
#include <string.h>
#include <stdlib.h>

void list_copy(list_template_t* new, list_template_t* old)
{
    assert(new);
    assert(old);

    memcpy(new, old, sizeof(list_template_t));
    new->data = malloc(new->buffsize);
    memcpy(new->data, old->data, new->buffsize);
}