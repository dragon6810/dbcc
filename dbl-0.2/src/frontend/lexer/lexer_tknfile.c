#include <frontend/lexer/lexer.h>

#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <assert/assert.h>
#include <cli/cli.h>
#include <list/list.h>
#include <math/math.h>
#include <srcfile/srcfile.h>

void lexer_tknfile_callphases(lexer_state_t* state, struct srcfile_s* srcfile)
{
    lexer_statesrcel_t stackbottom;

    memset(&stackbottom, 0, sizeof(lexer_state_t));
    strcpy(stackbottom.filename, srcfile->path);
    stackbottom.curline = stackbottom.curcolumn = 0;
    LIST_INITIALIZE(stackbottom.lines);

    LIST_INITIALIZE(state->tokens);
    LIST_INITIALIZE(state->srcstack);

    LIST_PUSH(state->srcstack, stackbottom);

    lexer_initialprocessing(state);
    lexer_tokenize(state);
}

bool lexer_tknfile(struct srcfile_s* srcfile)
{
    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    lexer_tknfile_callphases(&srcfile->lexer, srcfile);
    return true;
}
