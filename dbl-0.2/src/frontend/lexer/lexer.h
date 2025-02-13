#ifndef _lexer_h
#define _lexer_h

#include <stdbool.h>
#include <limits.h>

#include <list/list.h>

#define LEXER_MAXHARDTOKENLEN 64
#define LEXER_TOKENTYPE_STARTOFENUM LEXER_TOKENTYPE_EOF
#define LEXER_TOKENTYPE_ENDOFENUM LEXER_TOKENTYPE_CONSTANT
#define LEXER_TOKENTYPE_STARTOFPUNC LEXER_TOKENTYPE_OPENBRACKET
#define LEXER_TOKENTYPE_ENDOFPUNC LEXER_TOKENTYPE_BITNOTEQ
#define LEXER_TOKENTYPE_STARTOFPREPROC LEXER_TOKENTYPE_DEFINE
#define LEXER_TOKENTYPE_ENDOFPREPROC LEXER_TOKENTYPE_ENDIF
#define LEXER_TOKENTYPE_STARTOFKEYWORDS LEXER_TOKENTYPE_AUTO
#define LEXER_TOKENTYPE_ENDOFKEYWORDS LEXER_TOKENTYPE_WHILE
#define LEXER_TOKENTYPE_STARTOFDYNAMICS LEXER_TOKENTYPE_STRING
#define LEXER_TOKENTYPE_ENDOFDYNAMICS LEXER_TOKENTYPE_CONSTANT

/*
 * ================================
 *       FORWARD DECLARATIONS
 * ================================
*/

struct srcfile_s;

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct lexer_token_s lexer_token_t;
typedef struct lexer_state_s lexer_state_t;
typedef struct lexer_statesrcel_s lexer_statesrcel_t;
typedef struct lexer_barrier_s lexer_barrier_t;
typedef struct lexer_line_s lexer_line_t;

LIST_TYPE(lexer_line_t, list_lexer_line)
LIST_TYPE(lexer_barrier_t, list_lexer_barrier)
LIST_TYPE(lexer_token_t, list_lexer_token)
LIST_TYPE(lexer_statesrcel_t, list_lexer_statesrcel)
LIST_TYPE(lexer_state_t, list_lexer_state)

typedef enum
{
    LEXER_TOKENTYPE_INVALID=-1,

    /* EOF */
    LEXER_TOKENTYPE_EOF,
    
    /* Puncuation */
    LEXER_TOKENTYPE_OPENBRACKET,  /* [   */
    LEXER_TOKENTYPE_CLOSEBRACKET, /* ]   */
    LEXER_TOKENTYPE_OPENPARENTH,  /* (   */
    LEXER_TOKENTYPE_CLOSEPARENTH, /* )   */
    LEXER_TOKENTYPE_OPENBRACE,    /* {   */
    LEXER_TOKENTYPE_CLOSEBRACE,   /* }   */
    LEXER_TOKENTYPE_COMMA,        /* ,   */
    LEXER_TOKENTYPE_COLON,        /* :   */
    LEXER_TOKENTYPE_SEMICOLON,    /* ;   */
    LEXER_TOKENTYPE_ASTERISK,     /* *   */
    LEXER_TOKENTYPE_AMPERSAND,    /* &   */
    LEXER_TOKENTYPE_ASSIGN,       /* =   */
    LEXER_TOKENTYPE_NOT,          /* !   */
    LEXER_TOKENTYPE_NOTEQUAL,     /* !=  */
    LEXER_TOKENTYPE_EQUAL,        /* ==  */
    LEXER_TOKENTYPE_AND,          /* &&  */
    LEXER_TOKENTYPE_OR,           /* ||  */
    LEXER_TOKENTYPE_GREATER,      /* >   */
    LEXER_TOKENTYPE_LESS,         /* <   */
    LEXER_TOKENTYPE_GE,           /* >=  */
    LEXER_TOKENTYPE_LE,           /* <=  */
    LEXER_TOKENTYPE_QUESTION,     /* ?   */
    LEXER_TOKENTYPE_POUND,        /* #   */
    LEXER_TOKENTYPE_DBLPOUND,     /* ##  */
    LEXER_TOKENTYPE_PERIOD,       /* .   */
    LEXER_TOKENTYPE_PLUS,         /* +   */
    LEXER_TOKENTYPE_MINUS,        /* -   */
    LEXER_TOKENTYPE_MULT,         /* *   */
    LEXER_TOKENTYPE_DIV,          /* /   */
    LEXER_TOKENTYPE_MOD,          /* %   */
    LEXER_TOKENTYPE_INCREMENT,    /* ++  */
    LEXER_TOKENTYPE_DECREMENT,    /* --  */
    LEXER_TOKENTYPE_PLUSEQUALS,   /* +=  */
    LEXER_TOKENTYPE_MINUSEQUALS,  /* -=  */
    LEXER_TOKENTYPE_MULTEQUALS,   /* *=  */
    LEXER_TOKENTYPE_DIVEQUALS,    /* /=  */
    LEXER_TOKENTYPE_MODEQUALS,    /* %=  */
    LEXER_TOKENTYPE_BITSHIFTL,    /* <<  */
    LEXER_TOKENTYPE_BITSHIFTR,    /* >>  */
    LEXER_TOKENTYPE_BITAND,       /* &   */
    LEXER_TOKENTYPE_BITOR,        /* |   */
    LEXER_TOKENTYPE_BITXOR,       /* ^   */
    LEXER_TOKENTYPE_BITNOT,       /* ~   */
    LEXER_TOKENTYPE_BITSHIFTLEQ,  /* <<= */
    LEXER_TOKENTYPE_BITSHIFTREQ,  /* >>= */
    LEXER_TOKENTYPE_BITANDEQ,     /* &=  */
    LEXER_TOKENTYPE_BITOREQ,      /* |=  */
    LEXER_TOKENTYPE_BITXOREQ,     /* ^=  */
    LEXER_TOKENTYPE_BITNOTEQ,     /* ~=  */
    
    /* Pre-processor */
    LEXER_TOKENTYPE_DEFINE,       /* define  */
    LEXER_TOKENTYPE_INCLUDE,      /* include */
    LEXER_TOKENTYPE_IFDEF,        /* ifdef   */
    LEXER_TOKENTYPE_IFNDEF,       /* ifndef  */
    LEXER_TOKENTYPE_ENDIF,        /* endif   */

    /* Keywords */
    LEXER_TOKENTYPE_AUTO,         /* auto     */
    LEXER_TOKENTYPE_BREAK,        /* break    */
    LEXER_TOKENTYPE_CASE,         /* case     */
    LEXER_TOKENTYPE_CHAR,         /* char     */
    LEXER_TOKENTYPE_CONST,        /* const    */
    LEXER_TOKENTYPE_CONTINUE,     /* continue */
    LEXER_TOKENTYPE_DEFAULT,      /* default  */
    LEXER_TOKENTYPE_DO,           /* do       */
    LEXER_TOKENTYPE_DOUBLE,       /* double   */
    LEXER_TOKENTYPE_ELSE,         /* else     */
    LEXER_TOKENTYPE_ENUM,         /* enum     */
    LEXER_TOKENTYPE_EXTERN,       /* extern   */
    LEXER_TOKENTYPE_FLOAT,        /* float    */
    LEXER_TOKENTYPE_FOR,          /* for      */
    LEXER_TOKENTYPE_GOTO,         /* goto     */
    LEXER_TOKENTYPE_IF,           /* if       */
    LEXER_TOKENTYPE_INT,          /* int      */
    LEXER_TOKENTYPE_LONG,         /* long     */
    LEXER_TOKENTYPE_REGISTER,     /* register */
    LEXER_TOKENTYPE_RETURN,       /* return   */
    LEXER_TOKENTYPE_SHORT,        /* short    */
    LEXER_TOKENTYPE_SIGNED,       /* signed   */
    LEXER_TOKENTYPE_STATIC,       /* static   */
    LEXER_TOKENTYPE_STRUCT,       /* struct   */
    LEXER_TOKENTYPE_SWITCH,       /* switch   */
    LEXER_TOKENTYPE_TYPEDEF,      /* typedef  */
    LEXER_TOKENTYPE_UNION,        /* union    */
    LEXER_TOKENTYPE_UNSIGNED,     /* unsigned */
    LEXER_TOKENTYPE_VOID,         /* void     */
    LEXER_TOKENTYPE_VOLATILE,     /* volatile */
    LEXER_TOKENTYPE_WHILE,        /* while    */

    /* Dynamics */
    LEXER_TOKENTYPE_STRING,       /* "[string]"       */
    LEXER_TOKENTYPE_ANGLESTRING,  /* <[angle string]> */
    LEXER_TOKENTYPE_CHARCONSTANT, /* '[character]'    */
    LEXER_TOKENTYPE_IDENTIFIER,   /* [identifier]     */
    LEXER_TOKENTYPE_CONSTANT,     /* [constant]       */
} lexer_tokentype_e;

struct lexer_token_s
{
    char *val;
    lexer_tokentype_e type;
    
    char file[PATH_MAX];
    unsigned long int line, col;
};

struct lexer_statesrcel_s
{
    char filename[PATH_MAX];
    unsigned long int curline, curcolumn;
    list_lexer_line_t lines;
};

struct lexer_barrier_s
{
    unsigned long int position;
    unsigned long int line, column;
};

struct lexer_line_s
{
    char* str;
    list_lexer_barrier_t barriers;
};

struct lexer_state_s
{
    list_lexer_token_t tokens;
    list_lexer_statesrcel_t srcstack;
};

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

bool lexer_initialprocessing(lexer_state_t* state);
bool lexer_tokenize(lexer_state_t* state);
bool lexer_lexfile(struct srcfile_s* srcfile);
int lexer_tkntypetostring(lexer_tokentype_e type, char* name);
void lexer_printtokens(lexer_state_t* state);

#endif
