#ifndef _text_utils_h
#define _text_utils_h

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

bool txt_skipwhitespace(FILE* ptr);
void txt_skipline(FILE* ptr);
int is_valid_identifier_char(char c);

#endif