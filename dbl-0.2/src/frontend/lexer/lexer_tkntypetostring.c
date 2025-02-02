#include <frontend/lexer/lexer.h>

#include <string.h>

#pragma GCC diagnostic ignored "-Wswitch"

// I hate this. If i had a hashmap type I'd use that
int lexer_tkntypetostring(lexer_tokentype_e type, char* name)
{
    char str[LEXER_MAXHARDTOKENLEN];

    switch(type)
    {
    case LEXER_TOKENTYPE_EOF:
        strcpy(str, "eof");
        break;
    case LEXER_TOKENTYPE_OPENBRACKET:
        strcpy(str, "[");
        break;
    case LEXER_TOKENTYPE_CLOSEBRACKET:
        strcpy(str, "]");
        break;
    case LEXER_TOKENTYPE_OPENPARENTH:
        strcpy(str, "(");
        break;
    case LEXER_TOKENTYPE_CLOSEPARENTH:
        strcpy(str, ")");
        break;
    case LEXER_TOKENTYPE_OPENBRACE:
        strcpy(str, "{");
        break;
    case LEXER_TOKENTYPE_CLOSEBRACE:
        strcpy(str, "}");
        break;
    case LEXER_TOKENTYPE_COMMA:
        strcpy(str, ",");
        break;
    case LEXER_TOKENTYPE_COLON:
        strcpy(str, ":");
        break;
    case LEXER_TOKENTYPE_SEMICOLON:
        strcpy(str, ";");
        break;
    case LEXER_TOKENTYPE_POINTER:
        strcpy(str, "*");
        break;
    case LEXER_TOKENTYPE_AMPERSAND:
        strcpy(str, "&");
        break;
    case LEXER_TOKENTYPE_ASSIGN:
        strcpy(str, "=");
        break;
    case LEXER_TOKENTYPE_NOT:
        strcpy(str, "!");
        break;
    case LEXER_TOKENTYPE_NOTEQUAL:
        strcpy(str, "!=");
        break;
    case LEXER_TOKENTYPE_EQUAL:
        strcpy(str, "==");
        break;
    case LEXER_TOKENTYPE_AND:
        strcpy(str, "&&");
        break;
    case LEXER_TOKENTYPE_OR:
        strcpy(str, "||");
        break;
    case LEXER_TOKENTYPE_GREATER:
        strcpy(str, ">");
        break;
    case LEXER_TOKENTYPE_LESS:
        strcpy(str, "<");
        break;
    case LEXER_TOKENTYPE_GE:
        strcpy(str, ">=");
        break;
    case LEXER_TOKENTYPE_LE:
        strcpy(str, "<=");
        break;
    case LEXER_TOKENTYPE_THEN:
        strcpy(str, "?");
        break;
    case LEXER_TOKENTYPE_POUND:
        strcpy(str, "#");
        break;
    case LEXER_TOKENTYPE_PERIOD:
        strcpy(str, ".");
        break;
    case LEXER_TOKENTYPE_DEFINE:
        strcpy(str, "define");
        break;
    case LEXER_TOKENTYPE_INCLUDE:
        strcpy(str, "include");
        break;
    case LEXER_TOKENTYPE_IFDEF:
        strcpy(str, "ifdef");
        break;
    case LEXER_TOKENTYPE_IFNDEF:
        strcpy(str, "ifndef");
        break;
    case LEXER_TOKENTYPE_ENDIF:
        strcpy(str, "endif");
        break;
    case LEXER_TOKENTYPE_AUTO:
        strcpy(str, "auto");
        break;
    case LEXER_TOKENTYPE_BREAK:
        strcpy(str, "break");
        break;
    case LEXER_TOKENTYPE_CASE:
        strcpy(str, "case");
        break;
    case LEXER_TOKENTYPE_CHAR:
        strcpy(str, "char");
        break;
    case LEXER_TOKENTYPE_CONST:
        strcpy(str, "const");
        break;
    case LEXER_TOKENTYPE_CONTINUE:
        strcpy(str, "continue");
        break;
    case LEXER_TOKENTYPE_DEFAULT:
        strcpy(str, "default");
        break;
    case LEXER_TOKENTYPE_DO:
        strcpy(str, "do");
        break;
    case LEXER_TOKENTYPE_DOUBLE:
        strcpy(str, "double");
        break;
    case LEXER_TOKENTYPE_ELSE:
        strcpy(str, "else");
        break;
    case LEXER_TOKENTYPE_ENUM:
        strcpy(str, "enum");
        break;
    case LEXER_TOKENTYPE_EXTERN:
        strcpy(str, "extern");
        break;
    case LEXER_TOKENTYPE_FLOAT:
        strcpy(str, "float");
        break;
    case LEXER_TOKENTYPE_FOR:
        strcpy(str, "for");
        break;
    case LEXER_TOKENTYPE_GOTO:
        strcpy(str, "goto");
        break;
    case LEXER_TOKENTYPE_IF:
        strcpy(str, "if");
        break;
    case LEXER_TOKENTYPE_INT:
        strcpy(str, "int");
        break;
    case LEXER_TOKENTYPE_LONG:
        strcpy(str, "long");
        break;
    case LEXER_TOKENTYPE_REGISTER:
        strcpy(str, "register");
        break;
    case LEXER_TOKENTYPE_RETURN:
        strcpy(str, "return");
        break;
    case LEXER_TOKENTYPE_SHORT:
        strcpy(str, "short");
        break;
    case LEXER_TOKENTYPE_SIGNED:
        strcpy(str, "signed");
        break;
    case LEXER_TOKENTYPE_STATIC:
        strcpy(str, "static");
        break;
    case LEXER_TOKENTYPE_STRUCT:
        strcpy(str, "struct");
        break;
    case LEXER_TOKENTYPE_SWITCH:
        strcpy(str, "switch");
        break;
    case LEXER_TOKENTYPE_TYPEDEF:
        strcpy(str, "typedef");
        break;
    case LEXER_TOKENTYPE_UNION:
        strcpy(str, "union");
        break;
    case LEXER_TOKENTYPE_UNSIGNED:
        strcpy(str, "unsigned");
        break;
    case LEXER_TOKENTYPE_VOID:
        strcpy(str, "void");
        break;
    case LEXER_TOKENTYPE_VOLATILE:
        strcpy(str, "volatile");
        break;
    case LEXER_TOKENTYPE_WHILE:
        strcpy(str, "while");
        break;
    case LEXER_TOKENTYPE_STRING:
        strcpy(str, "string");
        break;
    case LEXER_TOKENTYPE_PLUS:
        strcpy(str, "+");
        break;
    case LEXER_TOKENTYPE_MINUS:
        strcpy(str, "-");
        break;
    case LEXER_TOKENTYPE_MULT:
        strcpy(str, "*");
        break;
    case LEXER_TOKENTYPE_DIV:
        strcpy(str, "/");
        break;
    case LEXER_TOKENTYPE_MOD:
        strcpy(str, "\%");
        break;
    case LEXER_TOKENTYPE_UNARYPLUS:
        strcpy(str, "+");
        break;
    case LEXER_TOKENTYPE_UNARYMINUS:
        strcpy(str, "-");
        break;
    case LEXER_TOKENTYPE_INCREMENT:
        strcpy(str, "++");
        break;
    case LEXER_TOKENTYPE_DECREMENT:
        strcpy(str, "--");
        break;
    case LEXER_TOKENTYPE_PLUSEQUALS:
        strcpy(str, "+=");
        break;
    case LEXER_TOKENTYPE_MINUSEQUALS:
        strcpy(str, "-=");
        break;
    case LEXER_TOKENTYPE_MULTEQUALS:
        strcpy(str, "*=");
        break;
    case LEXER_TOKENTYPE_DIVEQUALS:
        strcpy(str, "/=");
        break;
    case LEXER_TOKENTYPE_MODEQUALS:
        strcpy(str, "\%=");
        break;
    case LEXER_TOKENTYPE_BITSHIFTL:
        strcpy(str, "<<");
        break;
    case LEXER_TOKENTYPE_BITSHIFTR:
        strcpy(str, ">>");
        break;
    case LEXER_TOKENTYPE_BITAND:
        strcpy(str, "&");
        break;
    case LEXER_TOKENTYPE_BITOR:
        strcpy(str, "|");
        break;
    case LEXER_TOKENTYPE_BITNOT:
        strcpy(str, "~");
        break;
    case LEXER_TOKENTYPE_BITSHIFTLEQ:
        strcpy(str, "<<=");
        break;
    case LEXER_TOKENTYPE_BITSHIFTREQ:
        strcpy(str, ">>=");
        break;
    case LEXER_TOKENTYPE_BITANDEQ:
        strcpy(str, "&=");
        break;
    case LEXER_TOKENTYPE_BITOREQ:
        strcpy(str, "|=");
        break;
    case LEXER_TOKENTYPE_BITNOTEQ:
        strcpy(str, "~=");
        break;
    case LEXER_TOKENTYPE_IDENTIFIER:
        strcpy(str, "identifier");
        break;
    case LEXER_TOKENTYPE_CONSTANT:
        strcpy(str, "constant");
        break;
    default:
        str[0] = 0;
    }

    if(name)
        strcpy(name, str);

    return strlen(str);
}
