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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mcc.h"
#include "TestUtils.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "preprocessor.h"
#include "macro.h"

static void test_Define(void)
{
   const char *token_string = "#define TEST_MACRO 42\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
   printf("Test Define\n");

   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

   //should assert that we indeed got the right symbol defined.
   mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
   MCC_ASSERT(macro != NULL);
   mcc_TokenListIterator_t *valueIter = mcc_TokenListGetIterator(macro->tokens);
   mcc_Token_t *token = mcc_GetNextToken(valueIter);
   MCC_ASSERT(strncmp(token->text, "42", 2) == 0);
   mcc_TokenListDeleteIterator(valueIter);
   printf("ok\n");

   mcc_TokenListDelete(tokens);
   mcc_TokenListDelete(output);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_Undef(void)
{
   const char *token_string = "#define TEST_MACRO 42\n#undef TEST_MACRO";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   printf("Test Undef...");
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

   mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
   MCC_ASSERT(macro == NULL);
   printf("ok\n");

   mcc_TokenListDelete(output);
   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_DefineFunctionMacro(void)
{
   const char *token_string = "#define max(a, b) ( ((a) > (b)) ? (a) : (b) )\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

   printf("Test define function macro\n");
   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

   mcc_Macro_t *macro = mcc_ResolveMacro("max");
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(macro->is_function);
   mcc_TokenListIterator_t *argIter = mcc_TokenListGetIterator(macro->arguments);
   mcc_Token_t *current = mcc_GetNextToken(argIter);
   MCC_ASSERT(current->tokenType == TOK_IDENTIFIER);
   MCC_ASSERT(strncmp(current->text, "a", strlen(current->text)) == 0);
   current = mcc_GetNextToken(argIter);
   MCC_ASSERT(current->tokenType == TOK_IDENTIFIER);
   MCC_ASSERT(strncmp(current->text, "b", strlen(current->text)) == 0);
   current = mcc_GetNextToken(argIter);
   MCC_ASSERT(current == NULL);
   eral_ListDeleteIterator(argIter);
   printf("ok\n");

   mcc_TokenListDelete(output);
   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_NoWhitespaceFunctionCall(void)
{
   const char *token_string = "\
#define __GNUC_PREREQ(a,b) a+b\n\
#if __GNUC_PREREQ (4,3)\n\
   #define IF_MACRO\n\
#endif";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

   printf("Test No Whitespace function call\n");
   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

   mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
   MCC_ASSERT(macro != NULL);

   printf("ok\n");

   mcc_TokenListDelete(output);
   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_VariadicMacroFunctionDefinition(void)
{
   const char *token_string = "#define mcc_Error(...) __VA_ARGS__\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
   printf("Test Variadic Macro function definition...");

   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);
   mcc_Macro_t *macro = mcc_ResolveMacro("mcc_Error");
   MCC_ASSERT(macro->is_variadic);
   mcc_TokenListDelete(output);
   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

static void test_VariadicMacroFunctionUse(void)
{
   const char *token_string = "\
#define mcc_Error(...) __VA_ARGS__\n\
mcc_Error(\"Hello\");";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_InitialiseMacros();
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
   printf("Test Variadic Macro function use...");

   mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);
   MCC_ASSERT(eral_ListGetLength(output) == 3);
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(output);
   mcc_Token_t *tok = mcc_GetNextToken(iter);
   MCC_ASSERT(tok->tokenType == TOK_STR_CONST);
   tok = mcc_GetNextToken(iter);
   MCC_ASSERT(tok->tokenType == TOK_SYMBOL);
   MCC_ASSERT(tok->tokenIndex == SYM_SEMI_COLON);
   tok = mcc_GetNextToken(iter);
   MCC_ASSERT(tok->tokenType == TOK_EOL);

   mcc_TokenListDeleteIterator(iter);
   mcc_TokenListDelete(output);
   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   test_Define();
   test_Undef();
   test_DefineFunctionMacro();
   test_NoWhitespaceFunctionCall();
   test_VariadicMacroFunctionDefinition();
   test_VariadicMacroFunctionUse();
   return 0;
}
