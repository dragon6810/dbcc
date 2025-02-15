#include <cli/cli.h>

#include <stdio.h>
#include <string.h>

#define CLI_PRINTWELCOME_NEQUALS 32

void cli_printwelcome(void)
{
    char equals[CLI_PRINTWELCOME_NEQUALS+1];

    memset(equals, '=', sizeof(equals));
    equals[CLI_PRINTWELCOME_NEQUALS] = 0;
    printf("\n%s Deadbird Compiler %s%d.%d %s\n\n", equals, CLI_VERSION_IDENT, CLI_VERSION_MAJOR, CLI_VERSION_MINOR, equals);
}