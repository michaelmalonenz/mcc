/**
    mcc a lightweight compiler for developers, not machines
    Copyright (C) 2011 Michael Malone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"
#include "tokens.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "TestUtils.h"

#define NUM_TEST_CASES 5
const char *strings_to_tokenise[NUM_TEST_CASES] = { 
   "#include \"12/some_header.h\"\n",
   "char /*embedded comment*/ a = /*second embedded comment*/'\\0377';\n",
   "char a = '\\xF8';\n",
   "long foo = 1;",
   "int bar = 42>2 ? 42 : 58;"
};

const int32_t expected_num_tokens[NUM_TEST_CASES] = {
   5, 10, 10, 9, 19
};
#define LARGEST_NUM_TOKENS 19

const uint32_t expected_token_types[NUM_TEST_CASES][LARGEST_NUM_TOKENS] = {
   { TOK_PP_DIRECTIVE, TOK_WHITESPACE, TOK_LOCAL_FILE_INC, TOK_EOL, TOK_EOL },
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_CHAR_CONST, TOK_SYMBOL, TOK_EOL, TOK_EOL},
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_CHAR_CONST, TOK_SYMBOL, TOK_EOL, TOK_EOL},
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_SYMBOL, TOK_EOL},
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_OPERATOR, TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_SYMBOL, TOK_EOL}
};

const int expected_token_indices[NUM_TEST_CASES][LARGEST_NUM_TOKENS] = {
   { PP_INCLUDE, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX},
   { KEY_CHAR, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_EQUALS_ASSIGN, TOK_UNSET_INDEX, TOK_UNSET_INDEX, SYM_SEMI_COLON, TOK_UNSET_INDEX, TOK_UNSET_INDEX},
   { KEY_CHAR, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_EQUALS_ASSIGN, TOK_UNSET_INDEX, TOK_UNSET_INDEX, SYM_SEMI_COLON, TOK_UNSET_INDEX, TOK_UNSET_INDEX},
   { KEY_LONG, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_EQUALS_ASSIGN, TOK_UNSET_INDEX, TOK_UNSET_INDEX, SYM_SEMI_COLON, TOK_UNSET_INDEX},
   { KEY_INT, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_EQUALS_ASSIGN, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_GREATER_THAN, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_TERNARY_IF, TOK_UNSET_INDEX, TOK_UNSET_INDEX, TOK_UNSET_INDEX, OP_TERNARY_ELSE, TOK_UNSET_INDEX, TOK_UNSET_INDEX, SYM_SEMI_COLON, TOK_UNSET_INDEX }
};

int main(void)
{
   int i, j;
   for (i = 0; i < NUM_TEST_CASES; i++)
   {
      ssize_t tok_str_len = strlen(strings_to_tokenise[i]);
      mcc_TokenListIterator_t *tokenListIter;
      const char *tmp_filename = mcc_TestUtils_DumpStringToTempFile(
         strings_to_tokenise[i], tok_str_len);

      printf("***** Beginning Tokeniser test %d ******\n", i+1);
      mcc_FileOpenerInitialise();

      tokenListIter = mcc_TokenListGetIterator();
      mcc_TokeniseFile(tmp_filename, tokenListIter);
      mcc_TokenListDeleteIterator(tokenListIter);
      tokenListIter = mcc_TokenListGetIterator();

      for (j = 0; j < expected_num_tokens[i]; j++)
      {
         mcc_Token_t *token = mcc_GetNextToken(tokenListIter);
         MCC_ASSERT(token != NULL);
         printf("Expected token type: %s, Actual token type: %s\n",
                token_types[expected_token_types[i][j]],
                token_types[token->tokenType]);
         if (token->tokenType != expected_token_types[i][j])
         {
            mcc_DebugPrintToken(token);
         }
         if (token->tokenIndex != expected_token_indices[i][j])
         {
            printf("Expected index: %d, actual: %d\n",
               token->tokenIndex, expected_token_indices[i][j]);
         }
         MCC_ASSERT(token->tokenType == expected_token_types[i][j]);
         MCC_ASSERT(token->tokenIndex == expected_token_indices[i][j]);
      }

      MCC_ASSERT(mcc_GetNextToken(tokenListIter) == NULL);
      mcc_TokenListDeleteIterator(tokenListIter);
      mcc_FreeTokens();
      mcc_FileOpenerDelete();

      unlink(tmp_filename);
      printf("***** Finished Tokeniser test %d ******\n", i+1);
   }
   return 0;
}
