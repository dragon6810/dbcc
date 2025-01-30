#include <assert/assert.h>

#include <execinfo.h>
#include <unistd.h>
#include <limits.h>

#ifdef APPLE
    #include <mach-o/dyld.h>
#endif

void assert_printstacktrace(void)
{
    void **buff;
    char **symbols;
    unsigned int size;
    int stacksize;
    char progpath[PATH_MAX];
    unsigned int progpathlen;
    char *syscmd;
    void *baseaddr;
    FILE *pipe;
    char res[LINE_MAX];

    size = 16;
    buff = 0;
    while (1) 
    {
        if(buff)
            buff = realloc(buff, size * sizeof(void*));
        else
            buff = malloc(size * sizeof(void*));

        stacksize = backtrace(buff, size);
        if (stacksize < size)
            break;

        size <<= 2;
    }

    progpathlen = sizeof(progpath);
#ifdef APPLE
    _NSGetExecutablePath(progpath, &progpathlen);
#endif

    symbols = backtrace_symbols(buff, stacksize);
    fprintf(stderr, "Stack trace:\n");
    // start at 1 so you dont print this function in the call stack
    for (int i = 1; i < stacksize; i++)
    {
        fprintf(stderr, "%s\n", symbols[i]);

#ifdef APPLE
        baseaddr = _dyld_get_image_header(0);
        //printf("base adress: %p.\n", baseaddr);
        syscmd = malloc(snprintf(0, 0, "atos -o %s %p", progpath, buff[i] - baseaddr) + 1);
        sprintf(syscmd, "atos -o %s %p", progpath, buff[i] - baseaddr);
#endif
        pipe = popen(syscmd, "r");
        
        fgets(res, sizeof(res), pipe);
        //puts(res);

        pclose(pipe);
        free(syscmd);
    }
    
    free(symbols);
    free(buff);
}