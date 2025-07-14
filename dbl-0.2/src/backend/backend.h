#ifndef _backend_h
#define _backend_h

#include <stdint.h>

typedef struct executable_s executable_t;

struct executable_s
{
    uint8_t* text;
    uint64_t textsize;
    uint64_t entry;
};

#endif