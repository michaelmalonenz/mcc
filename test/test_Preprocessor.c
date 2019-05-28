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

static void test_Define(void)
{
   const char *token_string  = "#define TEST_MACRO 42\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   mcc_PreprocessCurrentTokens();

   //should assert that we indeed got the right symbol defined.
   mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(strncmp(macro->text, "42", 2) == 0);

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
}

static void test_NestedIf(void)
{
   const char *token_string  = "\
#define SOME_MACRO 1\n\
#if SOME_MACRO\n\
   #if SOME_OTHER_MACRO\n\
      #define TEST_MACRO 10\n\
   #else\n\
      #define TEST_MACRO 42\
   #endif\n\
#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   mcc_PreprocessCurrentTokens();

   //should assert that we indeed got the right symbol defined.
   mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(strncmp(macro->value, "42", 2) == 0);

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
}

static void test_IfNDef(void)
{
   const char *token_string  = "#ifndef SOME_MACRO\n#define SOME_MACRO\n#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   printf("Test IFNDEF\n");
   mcc_PreprocessCurrentTokens();

   mcc_Macro_t *macro = mcc_ResolveMacro("SOME_MACRO");
   MCC_ASSERT(macro != NULL); // It was defined, but value should be NULL
   printf("ok\n");

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_IfDef(void)
{
   const char *token_string  = "#define SOME_MACRO\n#ifdef IF_MACRO\n#define IF_MACRO\n#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   printf("Test IFDEF\n");
   mcc_PreprocessCurrentTokens();

   mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
   MCC_ASSERT(macro != NULL);
   printf("Test ok\n");

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_IfDef_Else(void)
{
   const char *token_string  = "#ifdef SOME_MACRO\n#define IF_MACRO\n#else\n#define ELSE_MACRO\n#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   printf("Test IFDEF else\n");
   mcc_PreprocessCurrentTokens();

   mcc_Macro_t *macro = mcc_ResolveMacro("ELSE_MACRO");
   MCC_ASSERT(macro != NULL);

   macro = mcc_ResolveMacro("IF_MACRO");
   MCC_ASSERT(macro == NULL);
   printf("ok\n");

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

static void test_IfDef_If(void)
{
   const char *token_string  = "#define SOME_MACRO\n#ifdef SOME_MACRO\n#define IF_MACRO\n#else\n#define ELSE_MACRO\n#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   printf("Test IFDEF if\n");
   mcc_PreprocessCurrentTokens();

   mcc_Macro_t *macro = mcc_ResolveMacro("ELSE_MACRO");
   MCC_ASSERT(macro == NULL);

   macro = mcc_ResolveMacro("IF_MACRO");
   MCC_ASSERT(macro != NULL);
   printf("ok\n");

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
   mcc_DeleteAllMacros();
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   test_Define();
   test_NestedIf();
   test_IfNDef();
   test_IfDef();
   test_IfDef_Else();
   test_IfDef_If();
   return 0;
}
