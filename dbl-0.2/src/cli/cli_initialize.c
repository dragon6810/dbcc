#include <cli/cli.h>

#include <stdio.h>
#include <string.h>

void cli_initialize_defaultincludedirs(void)
{
#ifdef LINUX
    const char *usrinc = "/usr/include";
#elif defined APPLE
    const char *usrinc = "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include";
#else
    const char *usrinc = "";
#endif

    char *cur;

    cur = strdup(usrinc);
    LIST_PUSH(cli_includedirs, cur);
}

void cli_initialize(void)
{
    LIST_INITIALIZE(cli_includedirs);
    LIST_INITIALIZE(cli_allunits);

    cli_initialize_defaultincludedirs();
}