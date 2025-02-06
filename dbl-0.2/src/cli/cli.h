#ifndef _cli_h
#define _cli_h

#include <stdbool.h>

#include <list/list.h>

#define CLI_VERSION_IDENT "i" /* i for in development */
#define CLI_VERSION_MAJOR 0
#define CLI_VERSION_MINOR 1

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

extern bool cli_verbose;
extern list_t cli_includedirs;

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void cli_initialize(void);
void cli_errornofile(const char* reason, const char* path);
void cli_printwelcome(void);
void cli_printusage(void);
void cli_processflag(const char* flag);

#endif