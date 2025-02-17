#include <frontend/lexer/lexer.h>

#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <cli/cli.h>
#include <srcfile/srcfile.h>
#include <std/assert/assert.h>
#include <std/list/list.h>
#include <std/math/math.h>
#include <std/profiler/profiler.h>

void lexer_tknfile_callphases(lexer_state_t* state, struct srcfile_s* srcfile)
{
    lexer_statesrcel_t stackbottom;

    memset(&stackbottom, 0, sizeof(lexer_state_t));
    strcpy(stackbottom.filename, srcfile->path);
    stackbottom.curline = stackbottom.curcolumn = 0;
    LIST_INITIALIZE(stackbottom.lines);
    LIST_INITIALIZE(stackbottom.tokens);

    LIST_INITIALIZE(state->srcstack);

    LIST_PUSH(state->srcstack, stackbottom);

    HASHMAP_INITIALIZE(state->defines, HASHMAP_BUCKETS_MEDIUM, string_lexer_define);

    lexer_initialprocessing(state);
    lexer_tokenize(state);
    lexer_preprocess(state);

    if(cli_verbose)
        lexer_printtokens(state);
}

bool lexer_lexfile(struct srcfile_s* srcfile)
{
    assert(srcfile);
    assert(srcfile->path);
    assert(srcfile->rawtext);

    profiler_push("Lex File");

    lexer_tknfile_callphases(&srcfile->lexer, srcfile);

    profiler_pop();

    return true;
}
