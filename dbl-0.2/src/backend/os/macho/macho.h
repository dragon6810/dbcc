#ifndef _macho_h
#define _macho_h

#include <stdbool.h>
#include <stdint.h>

#include <backend/backend.h>

#define MACHO_HEADERMAGIC 0xfeedfacf

#define MACHO_CPUTYPE_ARM 0x0c

#define MACHP_CPUSUBTYPE_ARM 0x00

#define MACHO_FILETYPE_OBJECT     1
#define MACHO_FILETYPE_EXECUTABLE 2
#define MACHO_FILETYPE_VMSHARED   3
#define MACHO_FILETYPE_CORE       4
#define MACHO_FILETYPE_PRELOAD    5
#define MACHO_FILETYPE_DYLIB      6
#define MACHO_FILETYPE_DYLINK     7
#define MACHO_FILETYPE_BUNDLE     8
#define MACHO_FILETYPE_DYLIBSTUB  9
#define MACHO_FILETYPE_DBG       10
#define MACHO_FILETYPE_KEXT      11
#define MACHO_FILETYPE_FILESET   12

#define MACHO_LOADCMDTYPE_SEGMENT 0x00000001
#define MACHO_LOADCMDTYPE_SYMTAB  0x00000002
#define MACHO_LOADCMDTYPE_SEG64   0x00000025
#define MACHO_LOADCMDTYPE_MAIN    0x80000028

#define MACHO_PAGESIZE 0x1000

typedef struct macho_header_s macho_header_t;
typedef struct macho_loadcmd_s macho_loadcmd_t;
typedef struct macho_loadcmd_seg64_s macho_loadcmd_seg64_t;
typedef struct macho_loadcmd_main_s macho_loadcmd_main_t;

struct macho_header_s
{
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmd;
    uint32_t cmdsize;
    uint32_t flags;
    uint32_t reserved;
};

struct macho_loadcmd_s
{
    uint32_t loadcmdtype;
    uint32_t len;
};

struct macho_loadcmd_seg64_s
{
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct macho_loadcmd_main_s
{
    uint32_t entryoff;
    uint32_t stacksize;
};

bool macho_writefile(executable_t* exec, const char* filename);

#endif