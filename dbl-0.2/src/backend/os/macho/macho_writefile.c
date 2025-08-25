#include <backend/os/macho/macho.h>

#include <string.h>

#include <std/assert/assert.h>

#include <cli/cli.h>

bool macho_writefile(executable_t* exec, const char* filename)
{
    FILE *ptr;
    macho_header_t header;
    macho_loadcmd_t loadcmd[2];
    macho_loadcmd_seg64_t seg64cmd;
    macho_loadcmd_main_t maincmd;

    assert(exec);
    assert(filename);

    header.magic = MACHO_HEADERMAGIC;
    header.cputype = MACHO_CPUTYPE_ARM;
    header.cpusubtype = MACHO_CPUSUBTYPE_ARM;
    header.filetype = MACHO_FILETYPE_EXECUTABLE;
    header.ncmd = 2;
    header.cmdsize = sizeof(loadcmd) + sizeof(seg64cmd) + sizeof(maincmd);
    header.flags = 0;
    header.reserved = 0;

    loadcmd[0].loadcmdtype = MACHO_LOADCMDTYPE_SEG64;
    loadcmd[0].len = sizeof(loadcmd[0]) + sizeof(seg64cmd);
    strcpy(seg64cmd.segname, "__TEXT");
    seg64cmd.vmaddr = 0x100000000; // start address
    seg64cmd.vmsize = (exec->textsize + MACHO_PAGESIZE - 1) & ~(MACHO_PAGESIZE - 1); // page-align
    seg64cmd.fileoff = sizeof(header) + sizeof(loadcmd) + sizeof(seg64cmd) + sizeof(maincmd);
    seg64cmd.filesize = exec->textsize;
    seg64cmd.maxprot = 7; // read, write, execute
    seg64cmd.initprot = 5; // read, execute
    seg64cmd.nsects = 0; // no sections for now
    seg64cmd.flags = 0;

    loadcmd[1].loadcmdtype = MACHO_LOADCMDTYPE_MAIN;
    loadcmd[1].len = sizeof(loadcmd[0]) + sizeof(maincmd);
    maincmd.entryoff = seg64cmd.fileoff;
    maincmd.stacksize = 0; // no stack for now

    ptr = fopen(filename, "wb");
    if(!ptr)
    {
        cli_errornofile("could not open file for writing", filename);
        return false;
    }

    fwrite(&header, sizeof(header), 1, ptr);
    fwrite(&loadcmd[0], sizeof(loadcmd[0]), 1, ptr);
    fwrite(&seg64cmd, sizeof(seg64cmd), 1, ptr);
    fwrite(&loadcmd[1], sizeof(loadcmd[1]), 1, ptr);
    fwrite(&maincmd, sizeof(maincmd), 1, ptr);
    fwrite(exec->text, exec->textsize, 1, ptr);
    fclose(ptr);

    return true;
}