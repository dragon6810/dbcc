#ifndef _assert_h
#define _assert_h

#include <stdio.h>
#include <stdlib.h>

inline void assert_printstacktrace(void);

#define assert(expr) ((expr) ? (void)0 : \
                      (fprintf(stderr, "Assertion Failed: \"%s\", file: \"%s\", line: %d.\n", \
                                #expr, __FILE__, __LINE__), \
                                assert_printstacktrace(), \
                                abort()))

#endif