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
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <string.h>

/**
 * This is a C File to hold the variables which contain the strings of keywords, operators and symbols
 * This is to make linkage easier than if they were in a header file (which needs to be #include'd
 * in several places)
 */
#include "tokens.h"
#include "fileBuffer.h"
#include "tokenList.h"

const char *token_types[NUM_TOK_TYPES] = { "Preprocessor directive", "Identifier", "Keyword", "Symbol",
                                           "Operator", "Number", "String Constant", "Character Constant",
                                           "System File Include", "Local File Include", "Whitespace",
                                           "End Of Line"};

const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
                                                                     "endif", "else", "elif", "undef", "error", "pragma",
                                                                     "#", "warning"};

size_t pp_strlens[NUM_PREPROCESSOR_DIRECTIVES];

/* The following char *[]'s need to be kept in sync with the corresponding enums in tokens.h */

const char *keywords[NUM_KEYWORDS] = { "auto", "break", "case", "char", "const",
                                       "continue", "default", "do", "double",
                                       "else", "enum", "extern", "float", "for",
                                       "goto", "if", "int", "long", "register",
                                       "return", "short", "signed", "sizeof",
                                       "static", "struct", "switch", "typedef",
                                       "union", "unsigned", "void", "volatile",
                                       "while"
#if MCC_C99_COMPATIBLE
                                       ,"_Bool", "_Complex", "_Imaginary",
                                       "inline", "restrict"
#endif
};

size_t keyword_strlens[NUM_KEYWORDS];

const char *operators[NUM_OPERATORS] = { "--", "++", "*=", "/=", "%=", "+=",
                                         "-=", "<<=", ">>=", "&=", "^=", "|=",
                                         "==", "!=", ">", "<", ">=", "<=", "&&",
                                         "||", "!", "&", "|", "^", "<<", ">>",
                                         "~", "sizeof", "&", "*", "?", ":", ".",
                                         "->", "=", ",", "+", "-", "/", "*", "%",
                                         "--", "++" };
size_t operator_strlens[NUM_OPERATORS];

const char *symbols[NUM_SYMBOLS] = { "{", "}", "[", "]", ";", "(", ")", "\"", "'", "\\"};
size_t symbol_strlens[NUM_SYMBOLS];

const char *number_types[NUMBER_OF_NUMBER_TYPES] = { "signed integer", "unsigned integer", "single precision float", "double precision float"};


static bool_t initialised;

static inline void init_tokens(void)
{
   int i;
   for (i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
   {
      pp_strlens[i] = strlen(preprocessor_directives[i]);
   }

   for (i = 0; i < NUM_KEYWORDS; i++)
   {
      keyword_strlens[i] = strlen(keywords[i]);
   }

   for (i = 0; i < NUM_SYMBOLS; i++)
   {
      symbol_strlens[i] = strlen(symbols[i]);
   }

   for (i = 0; i < NUM_OPERATORS; i++)
   {
      operator_strlens[i] = strlen(operators[i]);
   }

   initialised = TRUE;
}

static int get_token_impl(mcc_LogicalLine_t *line, int num_tokens,
                          const char **token_list, size_t *strlens_list)
{
   int i;
   if (!initialised)
   {
      init_tokens();
   }

   for (i = 0; i < num_tokens; i++)
   {
      if (strncmp(&line->string[line->index], token_list[i], strlens_list[i]) == 0)
      {
         return i;
      }
   }
   
   return -1;
}

PREPROC_DIRECTIVE mcc_GetPreprocessorDirective(mcc_LogicalLine_t *line)
{
   int pp_dir;

   pp_dir = get_token_impl(line, NUM_PREPROCESSOR_DIRECTIVES, 
                           preprocessor_directives, pp_strlens);

   if (pp_dir != -1)
   {
      return (PREPROC_DIRECTIVE) pp_dir;
   }
   else
   {
      return PP_NONE;
   }
}

MCC_SYMBOL mcc_GetSymbol(mcc_LogicalLine_t *line)
{
   int sym;

   sym = get_token_impl(line, NUM_SYMBOLS, symbols, symbol_strlens);

   if (sym != -1)
   {
      return (MCC_SYMBOL) sym;
   }
   else
   {
      return SYM_NONE;
   }
}


MCC_OPERATOR mcc_GetOperator(mcc_LogicalLine_t *line)
{
   int op;

   op = get_token_impl(line, NUM_OPERATORS, operators, operator_strlens);

   if (op != -1)
   {
      return (MCC_OPERATOR) op;
   }
   else
   {
      return OP_NONE;
   }
}

MCC_KEYWORD mcc_GetKeyword(mcc_LogicalLine_t *line)
{
   int key;

   key = get_token_impl(line, NUM_KEYWORDS, keywords, keyword_strlens);

   if (key != -1)
   {
      return (MCC_KEYWORD) key;
   }
   else
   {
      return KEY_NONE;
   }
}


#if MCC_DEBUG
void mcc_DebugPrintToken_Fn(uintptr_t token_ptr)
{
   mcc_DebugPrintToken((mcc_Token_t *) token_ptr);
}
#endif
