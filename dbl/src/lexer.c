#include "lexer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "error.h"
#include "bnf.h"
#include "textutils.h"

darr_t tokenfiles;

HashTable* create_defaults_hash (void)
{
    HashTable *ht = createHashTable();

    // Storage-class-specifier
    insert(ht, "auto", "storage-class-specifier");
    insert(ht, "register", "storage-class-specifier");
    insert(ht, "static", "storage-class-specifier");
    insert(ht, "extern", "storage-class-specifier");
    insert(ht, "typedef", "storage-class-specifier");

    // Type-specifier
    insert(ht, "void", "type-specifier");
    insert(ht, "char", "type-specifier");
    insert(ht, "short", "type-specifier");
    insert(ht, "int", "type-specifier");
    insert(ht, "long", "type-specifier");
    insert(ht, "float", "type-specifier");
    insert(ht, "double", "type-specifier");
    insert(ht, "signed", "type-specifier");
    insert(ht, "unsigned", "type-specifier");
    insert(ht, "struct", "struct-or-union"); 
    insert(ht, "union", "struct-or-union");
    insert(ht, "enum", "enum-specifier"); 

    // Type-qualifier
    insert(ht, "const", "type-qualifier");
    insert(ht, "volatile", "type-qualifier");

    // Operators (a selection, add more as needed)
    insert(ht, "=", "assignment-operator");
    insert(ht, "+=", "assignment-operator");
    insert(ht, "-=", "assignment-operator");
    insert(ht, "*=", "assignment-operator");
    insert(ht, "/=", "assignment-operator");
    insert(ht, "%=", "assignment-operator");
    insert(ht, "<<=", "assignment-operator");
    insert(ht, ">>=", "assignment-operator");
    insert(ht, "&=", "assignment-operator");
    insert(ht, "^=", "assignment-operator");
    insert(ht, "|=", "assignment-operator");
    insert(ht, "&&", "logical-and-expression"); 
    insert(ht, "||", "logical-or-expression");
    insert(ht, "==", "equality-expression");
    insert(ht, "!=", "equality-expression");
    insert(ht, "<", "relational-expression");
    insert(ht, ">", "relational-expression");
    insert(ht, "<=", "relational-expression");
    insert(ht, ">=", "relational-expression");
    insert(ht, "<<", "shift-expression");
    insert(ht, ">>", "shift-expression");
    insert(ht, "+", "additive-expression"); 
    insert(ht, "-", "additive-expression"); 
    insert(ht, "*", "multiplicative-expression");
    insert(ht, "/", "multiplicative-expression");
    insert(ht, "%", "multiplicative-expression"); 
    insert(ht, "&", "unary-operator"); 
    insert(ht, "*", "unary-operator"); 
    insert(ht, "~", "unary-operator");
    insert(ht, "!", "unary-operator");
    insert(ht, "++", "unary-expression"); 
    insert(ht, "--", "unary-expression"); 

    // Punctuation and keywords
    insert(ht, ";", "punctuation");
    insert(ht, ",", "punctuation");
    insert(ht, ":", "punctuation"); 
    insert(ht, "(", "punctuation");
    insert(ht, ")", "punctuation");
    insert(ht, "[", "punctuation");
    insert(ht, "]", "punctuation");
    insert(ht, "{", "punctuation");
    insert(ht, "}", "punctuation");
    insert(ht, ".", "punctuation"); 
    insert(ht, "->", "punctuation"); 
    insert(ht, "if", "selection-statement");
    insert(ht, "else", "selection-statement");
    insert(ht, "switch", "selection-statement");
    insert(ht, "case", "labeled-statement");
    insert(ht, "default", "labeled-statement");
    insert(ht, "while", "iteration-statement");
    insert(ht, "do", "iteration-statement");
    insert(ht, "for", "iteration-statement");
    insert(ht, "goto", "jump-statement");
    insert(ht, "continue", "jump-statement");
    insert(ht, "break", "jump-statement");
    insert(ht, "return", "jump-statement");
    insert(ht, "sizeof", "unary-expression");

    return ht;
}


void tkn_init(void) 
{
    darr_init(&tokenfiles, sizeof(tokenfile_t));
}

char* read_file_to_string(FILE *file) {
    if (file == NULL) {
        perror("Invalid file pointer");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        return NULL; 
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        return NULL; 
    }

    buffer[file_size] = '\0';
    return buffer; 
}

bool tkn_loadfile(FILE* ptr) 
{
    int c;
    char* filetext = read_file_to_string(ptr);
    tokenfile_t newfile;
    token_t newtoken;
    unsigned long int linenum, charnum;
    bool ignorerestofline;
    bool insidequotes;

    linenum = 1;
    charnum = 1;
    ignorerestofline = false;
    insidequotes = false;

    HashTable *defaults_hash = create_defaults_hash();

    newfile.filename = "test"; 
    darr_init(&newfile.tokens, sizeof(token_t));

    char* str = calloc(1, 1); 
    if (!str) {
        return false;
    }

    int i = 0;
    while (i<strlen(filetext)) 
    {

        if (filetext[i] == '\n') 
        {
            linenum++;
            charnum = 1;
            ignorerestofline = false;
            i++;
            continue;
        } 
            
        charnum++;

        if (ignorerestofline) 
        {
            i++;
            continue;
        }

        if (filetext[i] == '"') 
        {
            insidequotes = !insidequotes;
        }

        if (insidequotes) 
        {
            int len = strlen(str);
            char* temp = realloc(str, len + 2); 
            if (!temp) {
                free(str);
                return false;
            }
            str = temp;
            str[len] = filetext[i];
            str[len + 1] = '\0';
            i++;
            continue; 
        }

        // Comment handling
        if (filetext[i] == '/' && filetext[i + 1] == '/') 
        {
            ignorerestofline = true;
            i += 2; 
            continue;
        }

        // See if the string from i to j is a known token
        int j = 0;
        int matchlen = 0;
        char* matchstr = NULL;
        for (j = i; j < strlen(filetext); j++) 
        {
            char* substr = calloc(j - i + 1, 1);
            if (!substr) 
            {
                return false;
            }
            memcpy(substr, filetext + i, j - i);
            substr[j - i] = '\0';

            char* match = get(defaults_hash, substr);
            if (match) 
            {
                if (strlen(substr) > matchlen) 
                {
                    matchlen = strlen(substr);
                    if (matchstr) 
                    {
                        free(matchstr);
                    }
                    matchstr = substr;
                } 
                else 
                {
                    free(substr);
                }
            } 
            else 
            {
                free(substr);
            }
        }

        // If matchstr is not NULL, starts with a letter, and follows a letter, then it falls within a word
        // and should not be considered a token. For example, printf should parse as printf, not as pr, int, f.
        if (i>0 && matchstr && is_valid_identifier_char(filetext[i - 1]) && is_valid_identifier_char(filetext[i])) 
        {
            free(matchstr);
            matchstr = NULL;
        }

        // Now do the same at the end of the string
        if ((i+strlen(str)) < strlen(filetext) && matchstr && is_valid_identifier_char(filetext[i + strlen(str)]) && is_valid_identifier_char(filetext[i + strlen(str) + 1])) 
        {
            free(matchstr);
            matchstr = NULL;
        } 

        if (matchstr || isspace(filetext[i]))
        {
            if (strlen(str) > 0) 
            {
                newtoken.linenum = linenum;
                newtoken.charnum = charnum - strlen(str);
                newtoken.realnum = i - strlen(str);
                newtoken.payload = strdup(str); 
                newtoken.tokentype = "dynamic";

                printf("ingesting string %s\n", newtoken.payload);

                darr_push(&newfile.tokens, &newtoken);

                str = calloc(1, 1);
                if (!str) 
                {
                    return false;
                }
            }
        }

        if (matchstr)
        {
            newtoken.linenum = linenum;
            newtoken.charnum = charnum - strlen(matchstr);
            newtoken.realnum = i - strlen(matchstr);
            newtoken.payload = matchstr; 
            newtoken.tokentype = get(defaults_hash, matchstr);

            darr_push(&newfile.tokens, &newtoken);

            i += strlen(matchstr);
            continue;
        }

        if (isspace(filetext[i]))
        {
            i++;
            continue;
        }

        printf(" adding char %c\n", filetext[i]);

        int len = strlen(str);
        char* temp = realloc(str, len + 2); 
        if (!temp) 
        {
            free(str);
            return false;
        }
        str = temp;
        str[len] = filetext[i];
        str[len + 1] = '\0'; 

        i++;
    }

    // If there's remaining content in `str` after the loop, create a token for it
    if (strlen(str) > 0) {
        newtoken.linenum = linenum;
        newtoken.charnum = charnum - strlen(str);
        newtoken.realnum = i - strlen(str);
        newtoken.payload = strdup(str);
        newtoken.tokentype = "dynamic";

        printf("ingesting string %s\n", newtoken.payload);

        darr_push(&newfile.tokens, &newtoken);
    }

    free(str); // Free the allocated memory for `str`

    darr_push(&tokenfiles, &newfile);

    return true;
}

bool print_tokens(void) 
{
    int i, j;

    tokenfile_t* filesdata;
    token_t* tokensdata;
    tokenfile_t* file;
    token_t* token;

    filesdata = (tokenfile_t*)tokenfiles.data;
    for (i = 0; i < tokenfiles.len; i++) 
    {
        file = &filesdata[i];
        tokensdata = (token_t*)file->tokens.data;
        printf("File: %s\n", file->filename);
        for (j = 0; j < file->tokens.len; j++) 
        {
            token = &tokensdata[j];
            printf("Token: %s Token Type: %s Line: %d Column: %d\n", token->payload, token->tokentype, token->linenum, token->charnum);
        }
    }

    return true;
}