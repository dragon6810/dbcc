#include "incfile.h"
#include "otherincfile.h"

/* #include <string.h> */

#define MY_DEFINE (6 + 13 * 4)

int main(int argc, char** argv)
{
    int x;
    char c;
    char *str = "my string \
                 is cool.";

    #ifndef MY_DEFINE
        #ifdef OTHER_DEFINE
            int thisshouldnthappen;
        #endif
        int mydefineisdefined;
    #endif

    x = 1;

    /* heres a one-line comment */

    x = 0;
    x +=/* com */3;
    x *= -1;
    x--;/*
     * This is
     * a multi
     * line
     * comment
     * cool
    */x = 6+3;
    c = '3';
    c = '\n';

    return 0;
}
