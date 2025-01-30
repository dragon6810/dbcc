#include "textutils.h"

bool linestart = true;

bool txt_skipwhitespace(FILE* ptr)
{
    char c;
    bool did;

    did = false;
    while((c = fgetc(ptr)) <= 32 && c != EOF)
    {
        if(c == '\n')
            linestart = true;

        did = true;
    }

    if(did)
        linestart = false;

    if(c != EOF)
        fseek(ptr, -1, SEEK_CUR);
    return did;
}

void txt_skipline(FILE* ptr)
{
    char c;

    while((c = fgetc(ptr)) != '\n' && c != EOF);
    linestart = true;
}