#include "lexer_bnf.h"

#include <string.h>

#include "error.h"

tkn_bnf_tokenfile_t tkn_bnf_loadfile(char* filename)
{
    tkn_bnf_tokenfile_t tknfile;

    FILE* ptr;
    unsigned long int fsize;
    char *buff;

    ptr = fopen(filename, "r");
    if(!ptr)
        Error("can't open source file \"%s\".\n", filename);

    printf("opening source file \"%s\".\n", filename);
    tknfile.filename = strdup(filename);

    fseek(ptr, 0, SEEK_END);
    fsize = ftell(ptr);
    buff = malloc(fsize + 1);
    fseek(ptr, 0, SEEK_SET);
    fread(buff, 1, fsize, ptr);
    fclose(ptr);
    buff[fsize] = 0;

    puts(buff);

    free(buff);
    return tknfile;
}