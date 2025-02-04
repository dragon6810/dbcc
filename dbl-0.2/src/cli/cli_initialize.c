#include <cli/cli.h>

void cli_initialize_defaultincludedirs(void)
{
    const char *usrinc = "/usr/include";

    list_push(&cli_includedirs, &usrinc);
}

void cli_initialize(void)
{
    list_initialize(&cli_includedirs, sizeof(const char*));

    cli_initialize_defaultincludedirs();
}