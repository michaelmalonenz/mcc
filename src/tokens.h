/**
 Copyright (c) 2012, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
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
#include "config.h"
#include "mcc.h"
#include "liberal.h"

typedef enum TYPE { TOK_PP_DIRECTIVE, TOK_IDENTIFIER, TOK_KEYWORD, TOK_SYMBOL,
                    TOK_OPERATOR, TOK_NUMBER, TOK_STR_CONST, TOK_CHAR_CONST,
                    TOK_SYS_FILE_INC, TOK_LOCAL_FILE_INC, TOK_WHITESPACE, TOK_EOL,
                    NUM_TOK_TYPES, TOK_NONE } TOKEN_TYPE;
extern const char *token_types[NUM_TOK_TYPES];

typedef enum mcc_NumberType { UNSIGNED_INT, SIGNED_INT, FLOAT, DOUBLE, NUMBER_OF_NUMBER_TYPES } mcc_NumberType_t;

typedef union mcc_NumberContainer
{
   int32_t integer_s;
   uint32_t integer_u;
   float float_s;
   double float_d;
} mcc_NumberContainer_t;

typedef struct mcc_Number
{
   mcc_NumberContainer_t number;
   mcc_NumberType_t numberType;
} mcc_Number_t;

typedef struct token {
   char *text;
   TOKEN_TYPE tokenType;
   int tokenIndex;
   unsigned short fileno;
   int lineno;
   unsigned int line_index;
   mcc_Number_t number;
} mcc_Token_t;


/* The following enums need to be kept in sync with the corresponding char *[] */
typedef enum key_index { KEY_AUTO, KEY_BREAK, KEY_CASE, KEY_CHAR, KEY_CONST, 
                         KEY_CONTINUE, KEY_DEFAULT, KEY_DO, KEY_DOUBLE, KEY_ELSE,
                         KEY_ENUM, KEY_EXTERN, KEY_FLOAT, KEY_FOR, KEY_GOTO,
                         KEY_IF, KEY_INT, KEY_LONG, KEY_REGISTER, KEY_RETURN,
                         KEY_SHORT, KEY_SIGNED, KEY_SIZEOF, KEY_STATIC,
                         KEY_STRUCT, KEY_SWITCH, KEY_TYPEDEF, KEY_UNION,
                         KEY_UNSIGNED, KEY_VOID, KEY_VOLATILE, KEY_WHILE,
                         KEY_BOOL, KEY_COMPLEX, KEY_IMAGINARY, KEY_INLINE,
                         KEY_RESTRICT, 
                         NUM_KEYWORDS, KEY_NONE } MCC_KEYWORD;

extern const char *keywords[NUM_KEYWORDS];
extern size_t keyword_strlens[NUM_KEYWORDS];

typedef enum operator_index {OP_DECREMENT_POST, OP_INCREMENT_POST, OP_TIMES_EQUALS,
                             OP_DIVIDE_EQUALS, OP_MOD_EQUALS, OP_PLUS_EQUALS,
                             OP_MINUS_EQUALS, OP_L_SHIFT_EQUALS,
                             OP_R_SHIFT_EQUALS, OP_BITWISE_AND_EQUALS,
                             OP_BITWISE_EXCL_OR_EQUALS, OP_BITWISE_INCL_OR_EQUALS,
                             OP_COMPARE_TO, OP_NOT_EQUAL, OP_GREATER_EQUAL,
                             OP_LESS_EQUAL, OP_L_SHIFT, OP_R_SHIFT,
                             OP_GREATER_THAN, OP_LESS_THAN, OP_LOGICAL_AND,
                             OP_LOGICAL_INCL_OR, OP_NOT, OP_BITWISE_AND,
                             OP_BITWISE_INCL_OR, OP_BITWISE_EXCL_OR,
                             OP_NEGATE, OP_SIZEOF, OP_ADDRESS_OF, OP_DEREFERENCE,
                             OP_TERNARY_IF, OP_TERNARY_ELSE, OP_VARIADIC_ARGS,
                             OP_MEMBER_OF, OP_DEREF_MEMBER_OF, OP_EQUALS_ASSIGN,
                             OP_COMMA, OP_ADD, OP_MINUS, OP_DIVIDE, OP_MULTIPLY,
                             OP_MODULO, OP_DECREMENT_PRE, OP_INCREMENT_PRE,
                             NUM_OPERATORS, OP_NONE} MCC_OPERATOR;
extern const char *operators[NUM_OPERATORS];
extern size_t operator_strlens[NUM_OPERATORS];

typedef enum symbol_index {SYM_OPEN_BRACE, SYM_CLOSE_BRACE, SYM_OPEN_BRACKET,
                           SYM_CLOSE_BRACKET, SYM_SEMI_COLON, SYM_OPEN_PAREN,
                           SYM_CLOSE_PAREN, SYM_DOUBLE_QUOTE, SYM_SINGLE_QUOTE,
                           SYM_ESCAPE, NUM_SYMBOLS, SYM_NONE} MCC_SYMBOL;
extern const char *symbols[NUM_SYMBOLS];
extern size_t symbol_strlens[NUM_SYMBOLS];

/* The order here, is my guess at the relative frequency of each directive's
   use - so we can match earlier in the list, and hopefully speed up the
   pre-processing portion a little bit (you know, minus the include_next which
   needs to come before include) */
typedef enum pp_directives { PP_INCLUDE_NEXT, PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF,
                             PP_IF, PP_ENDIF, PP_ELSE, PP_ELIF, PP_UNDEF,
                             PP_ERROR, PP_PRAGMA, PP_JOIN, PP_WARNING, PP_STRINGIFY,
                             NUM_PREPROCESSOR_DIRECTIVES, PP_NONE }
   PREPROC_DIRECTIVE;

extern const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES];
extern size_t pp_strlens[NUM_PREPROCESSOR_DIRECTIVES];

extern const char *number_types[NUMBER_OF_NUMBER_TYPES];

MCC_SYMBOL mcc_GetSymbol(eral_LogicalLine_t *line);
MCC_OPERATOR mcc_GetOperator(eral_LogicalLine_t *line);
PREPROC_DIRECTIVE mcc_GetPreprocessorDirective(eral_LogicalLine_t *line);
MCC_KEYWORD mcc_GetKeyword(eral_LogicalLine_t * line);

void mcc_ExpectTokenType(const mcc_Token_t *token, TOKEN_TYPE tokenType, int index);

#define TOK_UNSET_INDEX -1

#if MCC_DEBUG
void mcc_DebugPrintToken_Fn(uintptr_t token_ptr);
#endif

#endif /* MCC_TOKENS_H_ */
