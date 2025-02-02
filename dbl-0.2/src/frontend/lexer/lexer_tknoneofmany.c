#include <frontend/lexer/lexer.h>

#include <stdarg.h>

bool lexer_tknoneofmany(lexer_tokentype_e tkn, int n, ...)
{
    int i;

    va_list args;

    va_start(args, n);
    for(i=0; i<n; i++)
    {
        if(tkn == va_arg(args, lexer_tokentype_e))
            return true;
    }

    return false;
}