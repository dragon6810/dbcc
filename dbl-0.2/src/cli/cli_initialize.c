#include <cli/cli.h>

#include <stdio.h>

void cli_initialize_defaultincludedirs(void)
{
#ifdef LINUX
    const char *usrinc = "/usr/include";
#elif defined APPLE
    const char *usrinc = "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include";
#else
    const char *usrinc = "";
#endif

    list_push(&cli_includedirs, &usrinc);
}

void cli_initialize(void)
{
    list_initialize(&cli_includedirs, sizeof(const char*));

    cli_initialize_defaultincludedirs();
}