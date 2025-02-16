#ifndef _hashmap_lexer_h
#define _hashmap_lexer_h

#include <hashmap/hashmap.h>
#include <hashmap/string/string.h>

bool hashmap_lexer_define_cmp(void* a, void* b);
void hashmap_lexer_define_cpy(void* dst, void* src);
void hashmap_lexer_define_free(void* p);

#endif