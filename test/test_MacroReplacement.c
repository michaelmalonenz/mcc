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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mcc.h"
#include "TestUtils.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "macro.h"

static void test_SimpleReplacement(void)
{
   const char *token_string  = "#define TEST_MACRO 42\nconst int meaningOfLife = TEST_MACRO;\n";
   const TOKEN_TYPE expectedOutputTokenTypes[10] = {
     TOK_KEYWORD, TOK_WHITESPACE, TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER,
     TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_SYMBOL
   };
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);
   printf("Test Simple Replacement...");

   mcc_TokenList_t *output = mcc_PreprocessCurrentTokens();
   mcc_TokenListIterator_t *outputIter = mcc_TokenListStandaloneGetIterator(output);
   mcc_Token_t *token;
   int i;

   for (i = 0; i < 10; i++)
   {
     token = mcc_GetNextToken(outputIter);
     printf("Expected token type: %s\n", token_types[expectedOutputTokenTypes[i]]);
     mcc_DebugPrintToken(token);
     MCC_ASSERT(token->tokenType == expectedOutputTokenTypes[i]);
   }
   mcc_TokenListDeleteIterator(outputIter);
   printf("ok\n");

   mcc_TokenListDeleteStandalone(output);
   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_FunctionReplacement(void)
{
   const char *token_string  = "\
#define max(a, b) a > b ? a : b\n\
int meaningOfLife = max(42, 4);\n";
   const TOKEN_TYPE expectedOutputTokenTypes[21] = {
     TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_WHITESPACE,
     TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_SYMBOL, TOK_EOL
   };
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);
   printf("Test Function Macro Replacement...");

   mcc_TokenList_t *output = mcc_PreprocessCurrentTokens();
   mcc_TokenListIterator_t *outputIter = mcc_TokenListStandaloneGetIterator(output);
   mcc_Token_t *token;
   int i;

   for (i = 0; i < 21; i++)
   {
     token = mcc_GetNextToken(outputIter);
     if (token->tokenType != expectedOutputTokenTypes[i])
     {
        printf("Expected token type: %s\n", token_types[expectedOutputTokenTypes[i]]);
        mcc_DebugPrintToken(token);
     }
     MCC_ASSERT(token->tokenType == expectedOutputTokenTypes[i]);
   }
   mcc_TokenListDeleteIterator(outputIter);
   printf("ok\n");

   mcc_TokenListDeleteStandalone(output);
   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

void test_BuiltinReplacement(void)
{
  const char *token_string = "int i = __STDC_VERSION__;";
  const TOKEN_TYPE expectedOutputTokenTypes[9] = {
     TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_SYMBOL, TOK_EOL
   };
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);
   printf("Test Builtin Macro Replacement...");

   mcc_TokenList_t *output = mcc_PreprocessCurrentTokens();
   mcc_TokenListIterator_t *outputIter = mcc_TokenListStandaloneGetIterator(output);
   mcc_Token_t *token;
   int i;

   for (i = 0; i < 9; i++)
   {
     token = mcc_GetNextToken(outputIter);
     if (token->tokenType != expectedOutputTokenTypes[i])
     {
        printf("Expected token type: %s\n", token_types[expectedOutputTokenTypes[i]]);
        mcc_DebugPrintToken(token);
     }
     MCC_ASSERT(token->tokenType == expectedOutputTokenTypes[i]);
   }
   mcc_TokenListDeleteIterator(outputIter);
   printf("ok\n");

   mcc_TokenListDeleteStandalone(output);
   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   test_SimpleReplacement();
   test_FunctionReplacement();
   test_BuiltinReplacement();
   return 0;
}
