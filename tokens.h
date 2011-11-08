#ifndef MCC_TOKENS_H_
#define MCC_TOKENS_H_

/* here are some extra keywords I should probably also support:
     DEF(TOK_CONST2, "__const")
     DEF(TOK_CONST3, "__const__")
     DEF(TOK_VOLATILE2, "__volatile")
     DEF(TOK_VOLATILE3, "__volatile__")
     DEF(TOK_SIGNED2, "__signed")
     DEF(TOK_SIGNED3, "__signed__")
     DEF(TOK_INLINE2, "__inline")
     DEF(TOK_INLINE3, "__inline__")
     DEF(TOK_RESTRICT2, "__restrict")
     DEF(TOK_RESTRICT3, "__restrict__")
     DEF(TOK_EXTENSION, "__extension__")
     DEF(TOK_ATTRIBUTE1, "__attribute")
     DEF(TOK_ATTRIBUTE2, "__attribute__")
     DEF(TOK_ALIGNOF1, "__alignof")
     DEF(TOK_ALIGNOF2, "__alignof__")
     DEF(TOK_TYPEOF1, "typeof")
     DEF(TOK_TYPEOF2, "__typeof")
     DEF(TOK_TYPEOF3, "__typeof__")
     DEF(TOK_LABEL, "__label__")
     DEF(TOK_ASM1, "asm")
     DEF(TOK_ASM2, "__asm")
     DEF(TOK_ASM3, "__asm__")
*/
#include "mcc.h"
#include "config.h"
#include "fileBuffer.h"

typedef enum TYPE { TOK_PP_DIRECTIVE, TOK_IDENTIFIER, TOK_KEYWORD, TOK_SYMBOL,
                    TOK_OPERATOR, TOK_NUMBER, TOK_STR_CONST, TOK_CHAR_CONST,
                    TOK_SYS_FILE_INC, TOK_LOCAL_FILE_INC, TOK_WHITESPACE,
                    TOK_NONE } TOKEN_TYPE;
 
typedef struct token {
   char *text;
   TOKEN_TYPE tokenType;
   int tokenIndex;
   unsigned short fileno;
   int lineno;
   int line_index;
   struct token *next;
   struct token *previous;
} mcc_Token_t;

/* The following enums need to be kept in sync with the corresponding char *[] */
typedef enum key_index { KEY_AUTO, KEY_BREAK, KEY_CASE, KEY_CHAR, KEY_CONST, 
                         KEY_CONTINUE, KEY_DEFAULT, KEY_DO, KEY_DOUBLE, KEY_ELSE,
                         KEY_ENUM, KEY_EXTERN, KEY_FLOAT, KEY_FOR, KEY_GOTO,
                         KEY_IF, KEY_INT, KEY_LONG, KEY_REGISTER, KEY_RETURN,
                         KEY_SHORT, KEY_SIGNED, KEY_SIZEOF, KEY_STATIC,
                         KEY_STRUCT, KEY_SWITCH, KEY_TYPEDEF, KEY_UNION,
                         KEY_UNSIGNED, KEY_VOID, KEY_VOLATILE, KEY_WHILE,
#if MCC_C99_COMPATIBLE
                         KEY_BOOL, KEY_COMPLEX, KEY_IMAGINARY, KEY_INLINE,
                         KEY_RESTRICT, 
#endif
                         NUM_KEYWORDS, KEY_NONE } MCC_KEYWORD;

extern const char *keywords[NUM_KEYWORDS];
extern size_t keyword_strlens[NUM_KEYWORDS];

typedef enum operator_index {OP_DECREMENT, OP_INCREMENT, OP_TIMES_EQUALS,
                             OP_DIVIDE_EQUALS, OP_MOD_EQUALS, OP_PLUS_EQUALS,
                             OP_MINUS_EQUALS, OP_L_SHIFT_EQUALS,
                             OP_R_SHIFT_EQUALS, OP_BITWISE_AND_EQUALS,
                             OP_BITWISE_EXCL_OR_EQUALS, OP_BITWISE_INCL_OR_EQUALS,
                             OP_COMPARE_TO, OP_NOT_EQUAL, OP_GREATER_THAN, 
                             OP_LESS_THAN, OP_GREATER_EQUAL, OP_LESS_EQUAL, 
                             OP_LOGICAL_AND, OP_LOGICAL_EXCL_OR,
                             OP_LOGICAL_INCL_OR, OP_NOT, OP_BITWISE_AND, 
                             OP_BITWISE_INCL_OR, OP_BITWISE_EXCL_OR, OP_L_SHIFT,
                             OP_R_SHIFT, OP_NEGATE, OP_SIZEOF, OP_ADDRESS_OF, 
                             OP_TERNARY_IF, OP_TERNARY_ELSE, OP_MEMBER_OF,
                             OP_DEREF_MEMBER_OF, OP_EQUALS_ASSIGN, OP_COMMA,
                             OP_ADD, OP_MINUS, OP_DIVIDE, OP_MULTIPLY, OP_MODULO,
                             NUM_OPERATORS, OP_NONE} MCC_OPERATOR;
extern const char *operators[NUM_OPERATORS];
extern size_t operator_strlens[NUM_OPERATORS];

typedef enum symbol_index {SYM_OPEN_BRACE, SYM_CLOSE_BRACE, SYM_OPEN_BRACKET,
                           SYM_CLOSE_BRACKET, SYM_SEMI_COLON, SYM_OPEN_PAREN,
                           SYM_CLOSE_PAREN, SYM_DOUBLE_QUOTE, SYM_SINGLE_QUOTE,
                           SYM_ESCAPE, SYM_OPEN_SYS_INCL, SYM_CLOSE_SYS_INCL,
                           SYM_LOCAL_INCL, NUM_SYMBOLS, SYM_NONE} MCC_SYMBOL;
extern const char *symbols[NUM_SYMBOLS];
extern size_t symbol_strlens[NUM_SYMBOLS];

/* The order here, is my guess at the relative frequency of each directive's
   use - so we can match earlier in the list, and hopefully speed up the
   pre-processing portion a little bit */
typedef enum pp_directives { PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF,
                             PP_IF, PP_ENDIF, PP_ELSE, PP_ELIF, PP_UNDEF,
                             PP_ERROR, PP_PRAGMA, PP_JOIN,
                             NUM_PREPROCESSOR_DIRECTIVES, PP_NONE }
   PREPROC_DIRECTIVE;

extern const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES];
extern size_t pp_strlens[NUM_PREPROCESSOR_DIRECTIVES];


MCC_SYMBOL mcc_GetSymbol(mcc_LogicalLine_t *line);
MCC_OPERATOR mcc_GetOperator(mcc_LogicalLine_t *line);
PREPROC_DIRECTIVE mcc_GetPreprocessorDirective(mcc_LogicalLine_t *line);
MCC_KEYWORD mcc_GetKeyword(mcc_LogicalLine_t * line);


/**
 * @param text      The physical text of the token as in the source file
 *
 * @param text_len  The length of the physical text
 *
 * @param type      The type of token to create
 *
 * @param lineno    The current line number of the file where the token was found
 */
mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const int lineno);

/**
 * @param lineno The line number in the file
 *
 * Creates a whitespace token and adds it in the current position.
 */
void mcc_CreateAndAddWhitespaceToken(const int lineno);

void mcc_AddToken(mcc_Token_t *token);
void mcc_FreeTokens(void);
mcc_Token_t *mcc_ConCatTokens(mcc_Token_t *first, mcc_Token_t *second, TOKEN_TYPE newType);
mcc_Token_t *mcc_GetNextToken(void);
#endif /* MCC_TOKENS_H_ */
