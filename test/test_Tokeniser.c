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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"
#include "tokens.h"
#include "tokenList.h"
#include "toolChainCommands.h"

#define NUM_TEST_CASES 3
const char *strings_to_tokenise[NUM_TEST_CASES] = { 
   "#include \"12/some_header.h\"\n",
   "char a = '\\0377';\n",
   "char a = '\\xFF';\n"
};

const int32_t expected_num_tokens[NUM_TEST_CASES] = {
   4, 9, 9
};
#define LARGEST_NUM_TOKENS 9

const uint32_t expected_token_types[NUM_TEST_CASES][LARGEST_NUM_TOKENS] = {
   { TOK_PP_DIRECTIVE, TOK_WHITESPACE, TOK_LOCAL_FILE_INC, TOK_EOL },
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_CHAR_CONST, TOK_SYMBOL, TOK_EOL},
   { TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_CHAR_CONST, TOK_SYMBOL, TOK_EOL}
};

const int expected_token_indices[NUM_TEST_CASES][LARGEST_NUM_TOKENS] = {
   { PP_INCLUDE, 0, 0, 0},
   { KEY_CHAR, 0, 0, 0, OP_EQUALS_ASSIGN, 0, 0, SYM_SEMI_COLON, 0},
   { KEY_CHAR, 0, 0, 0, OP_EQUALS_ASSIGN, 0, 0, SYM_SEMI_COLON, 0}
};


char tmp_filename[L_tmpnam];

int main(int UNUSED(argc), char UNUSED(**argv))
{
   int i, j;
   for (i = 0; i < NUM_TEST_CASES; i++)
   {
      int temp_fd = 0;
      ssize_t bytes_written = 0;
      ssize_t tok_str_len = strlen(strings_to_tokenise[i]);
      mcc_TokenListIterator_t *tokenListIter;

      strncpy(tmp_filename, "mcc_test_XXXXXX", L_tmpnam);
      MCC_ASSERT(mkstemp(tmp_filename) != -1);

      temp_fd = open(tmp_filename, O_CREAT | O_WRONLY, S_IWUSR);
      MCC_ASSERT(temp_fd != -1);

      bytes_written = write(temp_fd, strings_to_tokenise[i], tok_str_len);
      while (bytes_written != tok_str_len)
      {
         bytes_written += write(temp_fd, &strings_to_tokenise[i][bytes_written],
                                tok_str_len - bytes_written);
      }

      close(temp_fd);

      mcc_FileOpenerInitialise();

      tokenListIter = mcc_GetTokenListIterator();
      mcc_TokeniseFile(tmp_filename, tokenListIter);
      mcc_DeleteTokenListIterator(tokenListIter);
      tokenListIter = mcc_GetTokenListIterator();

      for (j = 0; j < expected_num_tokens[i]; j++)
      {
         mcc_Token_t *token = mcc_GetNextToken(tokenListIter);
         MCC_ASSERT(token != NULL);
         printf("Expected token type: %s, Actual token type: %s\n",
                token_types[expected_token_types[i][j]],
                token_types[token->tokenType]);
         MCC_ASSERT(token->tokenType == expected_token_types[i][j]);
         MCC_ASSERT(token->tokenIndex == expected_token_indices[i][j]);
      }

      MCC_ASSERT(mcc_GetNextToken(tokenListIter) == NULL);
      mcc_DeleteTokenListIterator(tokenListIter);
      mcc_FreeTokens();
      mcc_FileOpenerDelete();

      unlink(tmp_filename);
   }
   return 0;
}
