#ifndef _list_h
#define _list_h

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct list_s list_t;

struct list_s
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

void list_initialize(list_t* list, unsigned long int elsize);
void list_resize(list_t* list, unsigned long int size);
void* list_push(list_t* list, void* element);
void list_pop(list_t* list, void* element);
void list_fromspan(list_t* new, list_t* list, unsigned long int start, unsigned long int end);
void list_copy(list_t* new, list_t* old);
void list_shuffle(list_t* out, list_t* list);
void list_free(list_t* list);

#endif
