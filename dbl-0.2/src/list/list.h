#ifndef _list_h
#define _list_h

/*
 * ================================
 *             MACROS
 * ================================
*/

#define LIST_FETCH(list, type, index) (((type*)list.data)[index])

#define LIST_TYPE(type) struct  \ 
{                               \
    unsigned long int size;     \
    unsigned long int buffsize; \
    unsigned long int elsize;   \
    type* data;                 \
}

#define LIST_INIT(list) do                        \
{                                                 \
    (list).size = 0;                              \
    (list).buffsize = 0;                          \
    (list).elsize = sizeof(typeof(*(list).data)); \
    (list).data = NULL;                           \
} while(0)

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct list_template_s list_template_t;

struct list_template_s
{
    unsigned long int size;
    unsigned long int buffsize;
    unsigned long int elsize;
    void *data;
};

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void list_initialize(list_template_t* list, unsigned long int elsize);
void list_resize(list_template_t* list, unsigned long int size);
void* list_push(list_template_t* list, void* element);
void list_pop(list_template_t* list, void* element);
void list_remove(list_template_t* list, unsigned long int index);
void list_fromspan(list_template_t* new, list_template_t* list, unsigned long int start, unsigned long int end);
void list_copy(list_template_t* new, list_template_t* old);
void list_shuffle(list_template_t* out, list_template_t* list);
void list_free(list_template_t* list);

#endif
