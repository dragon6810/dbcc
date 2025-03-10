#ifndef _cli_h
#define _cli_h

#include <stdarg.h>
#include <stdbool.h>

#include <std/list/list.h>
#include <std/list/list_types.h>

#include <srcfile/srcfile.h>

#define CLI_VERSION_IDENT "i" /* i for in development */
#define CLI_VERSION_MAJOR 0
#define CLI_VERSION_MINOR 1

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

extern bool cli_verbose;
extern list_str_t cli_includedirs;
extern list_srcfile_t cli_allunits;

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void cli_initialize(void);
void cli_errornofile(const char* reason, const char* path);
void cli_errorsyntax(const char* file, unsigned long int line, unsigned long int column, const char* format, ...);
void cli_verrorsyntax(const char* file, unsigned long int line, unsigned long int column, const char* format, va_list args);
void cli_printwelcome(void);
void cli_printusage(void);
void cli_processflag(const char* flag);

#endif