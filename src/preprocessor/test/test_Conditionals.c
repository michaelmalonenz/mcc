/**
 Copyright (c) 2019, Michael Malone
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
#include "preprocessor.h"
#include "toolChainCommands.h"
#include "tokenList.h"
#include "macro.h"
#include "TestUtils.h"

static void test_NestedIf(void)
{
    const char *token_string = "\
#define SOME_MACRO\n\
#ifdef SOME_MACRO\n\
   #ifdef SOME_OTHER_MACRO\n\
      #define TEST_MACRO 10\n\
   #else\n\
      #define TEST_MACRO 42\n\
   #endif\n\
#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    printf("Test nested IFDEF\n");
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    //should assert that we indeed got the right symbol defined.
    mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
    MCC_ASSERT(macro != NULL);
    mcc_TokenListIterator_t *valueIter = mcc_TokenListGetIterator(macro->tokens);
    mcc_Token_t *token = mcc_GetNextToken(valueIter);
    MCC_ASSERT(strncmp(token->text, "42", 2) == 0);
    mcc_TokenListDeleteIterator(valueIter);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_ReallyNestedIf(void)
{
    const char *token_string = "\
#ifdef SOME_MACRO\n\
   #ifdef SOME_OTHER_MACRO\n\
      #define TEST_MACRO 10\n\
   #else\n\
      #define TEST_MACRO 42\n\
   #endif\n\
   #ifndef YET_ANOTHER_MACRO\n\
      #define TEST_MACRO 54\n\
   #endif\n\
#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    printf("Test really nested IFDEF\n");
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

static void test_IfNDef(void)
{
    const char *token_string = "#ifndef SOME_MACRO\n#define SOME_MACRO\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IFNDEF\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("SOME_MACRO");
    MCC_ASSERT(macro != NULL); // It was defined, but value should be NULL
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_IfDef(void)
{
    const char *token_string = "#define SOME_MACRO 1\n#ifdef SOME_MACRO\n#define IF_MACRO 2\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IFDEF\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro != NULL);
    printf("Test ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_IfDef_Else(void)
{
    const char *token_string = "#ifdef SOME_MACRO\n#define IF_MACRO 1\n#else\n#define ELSE_MACRO 2\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IFDEF else\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro == NULL);

    macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro != NULL);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_IfDef_If(void)
{
    const char *token_string = "#define SOME_MACRO 3\n#ifdef SOME_MACRO\n#define IF_MACRO 2\n#else\n#define ELSE_MACRO 3\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IFDEF if\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro == NULL);

    macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro != NULL);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

void test_If(void)
{
    const char *token_string = "#if (1 + 1)\n#define IF_MACRO\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IF if\n");
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

void test_If_Else(void)
{
    const char *token_string = "#if (1 - 1)\n#define IF_MACRO\n#else\n#define ELSE_MACRO\n#endif\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IF else\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro == NULL);

    macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro != NULL);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_If_ComplexMacroCondition(void)
{
    const char *token_string = "\
#define SOME_OTHER_MACRO 1\n\
#if defined SOME_MACRO && SOME_MACRO || SOME_OTHER_MACRO\n\
 #define IF_MACRO\n\
#else\n\
 #define ELSE_MACRO\n\
#endif";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IF Complex Macro condition\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro != NULL);

    macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro == NULL);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_If_BuiltinDefines(void)
{
    const char *token_string = "\
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L \n\
 #define IF_MACRO\n\
#elif 0\n\
 #define ELSE_MACRO\n\
#endif";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test IF Builtin Defines\n");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro != NULL);

    macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro == NULL);
    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

static void test_BuiltinMacroIfElif(void)
{
    const char *token_string = "\
#if __WORDSIZE == 32\n\
#define IF_MACRO\n\
#elif __WORDSIZE == 64\n\
#define ELIF_MACRO\n\
#else\n\
#error Builtin Macro If Elif Test Failed\n\
#endif";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

    printf("Test Builtin Macro If Elif...");
    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);

    mcc_Macro_t *macro = mcc_ResolveMacro("IF_MACRO");
    MCC_ASSERT(macro == NULL);

    macro = mcc_ResolveMacro("ELIF_MACRO");
    MCC_ASSERT(macro != NULL);

    macro = mcc_ResolveMacro("ELSE_MACRO");
    MCC_ASSERT(macro == NULL);

    printf("ok\n");

    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
}

int main(void)
{
    test_IfDef();
    test_IfDef_Else();
    test_IfDef_If();
    test_IfNDef();
    test_NestedIf();
    test_ReallyNestedIf();
    test_If();
    test_If_Else();
    test_If_ComplexMacroCondition();
    test_If_BuiltinDefines();
    test_BuiltinMacroIfElif();
    return EXIT_SUCCESS;
}