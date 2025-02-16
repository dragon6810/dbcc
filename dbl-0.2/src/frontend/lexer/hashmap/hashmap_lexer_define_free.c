#include <frontend/lexer/lexer.h>

#include <assert/assert.h>

void hashmap_lexer_define_free(void* p)
{
    int i;

    lexer_define_t *define;

    define = (lexer_define_t*) p;

    assert(define);
    assert(define->name);
    
    free(define->name);

    for(i=0; i<define->tokens.size; i++)
        free(define->tokens.data[i].val);
    LIST_FREE(define->tokens);
}