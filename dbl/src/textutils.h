#ifndef _text_utils_h
#define _text_utils_h

#include <stdio.h>
#include <stdbool.h>

extern bool linestart;

bool txt_skipwhitespace(FILE* ptr);
void txt_skipline(FILE* ptr);

#endif