#include "incfile.ha"
#include "otherincfile.h"

/* #include <string.h> */

#define MY_DEFINE (6 + 13)

int main(int argc, char** argv)
{
    int x;
    char c;
    char *str = "my string \
                 is cool.";

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
    */x += MY_DEFINE;
    c = '3';
    c = '\n';

    return 0;
}
