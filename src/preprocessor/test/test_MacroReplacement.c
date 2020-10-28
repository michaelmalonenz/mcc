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
#include <stdbool.h>

#include "mcc.h"
#include "TestUtils.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "preprocessor.h"
#include "macro.h"

#define NUM_TEST_CASES 4
const char *token_strings[NUM_TEST_CASES] = {
    "#define TEST_MACRO 42\nconst int meaningOfLife = TEST_MACRO;\n",
    "#define max(a, b) a > b ? a : b\nint meaningOfLife = max(42, 4);\n",
    "#define fourty_two() 42\nint meaningOfLife = fourty_two();\n",
    "int i = __STDC_VERSION__;",
};
const int32_t expected_num_tokens[NUM_TEST_CASES] = {
    10, 21, 9, 9};

#define LARGEST_NUM_TOKENS 21
const uint32_t expected_token_types[NUM_TEST_CASES][LARGEST_NUM_TOKENS] = {
    {TOK_KEYWORD, TOK_WHITESPACE, TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE,
     TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_SYMBOL},
    {TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_WHITESPACE,
     TOK_OPERATOR, TOK_WHITESPACE, TOK_NUMBER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_SYMBOL, TOK_EOL},
    {TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_SYMBOL, TOK_EOL},
    {TOK_KEYWORD, TOK_WHITESPACE, TOK_IDENTIFIER, TOK_WHITESPACE, TOK_OPERATOR, TOK_WHITESPACE,
     TOK_NUMBER, TOK_SYMBOL, TOK_EOL},
};

static void test_Implementation(void)
{
    int i;
    for (i = 0; i < NUM_TEST_CASES; i++)
    {
        const char *token_string = token_strings[i];
        const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                              strlen(token_string));
        mcc_InitialiseMacros();
        mcc_FileOpenerInitialise();
        mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
        printf("Test Macro Replacement %d...", i + 1);

        mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);
        mcc_TokenListIterator_t *outputIter = mcc_TokenListGetIterator(output);
        mcc_Token_t *token;
        int j;

        for (j = 0; j < expected_num_tokens[i]; j++)
        {
            token = mcc_GetNextToken(outputIter);
            if (token->tokenType != expected_token_types[i][j])
            {
                printf("Expected token type at index [%d][%d]: %s\n",
                        i, j, token_types[expected_token_types[i][j]]);
                mcc_DebugPrintToken(token);
            }
            MCC_ASSERT(token->tokenType == expected_token_types[i][j]);
        }
        mcc_TokenListDeleteIterator(outputIter);
        printf("ok\n");

        mcc_TokenListDelete(output);
        mcc_TokenListDelete(tokens);
        mcc_FileOpenerDelete();
        unlink(file);
        mcc_DeleteAllMacros();
    }
}

static void test_SemiRecursiveMacroFunction(void)
{
    const char *token_string = "\
#define __FEAT_USE(F) __FEAT_USE ## F\n\
__FEAT_USE(_SEMI_MACRO)";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
    printf("Test Semi Recursive Macro Function...");

    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);
    mcc_TokenListIterator_t *outputIter = mcc_TokenListGetIterator(output);

    mcc_Token_t *token = mcc_GetNextToken(outputIter);
    MCC_ASSERT(token->tokenType == TOK_IDENTIFIER);
    MCC_ASSERT(strncmp(token->text, "__FEAT_USE_SEMI_MACRO", strlen(token->text)) == 0);

    mcc_TokenListDeleteIterator(outputIter);
    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
    printf("ok\n");
}

static void test_MultipleLongListReplacement(void)
{
    const char *token_string = "\
#define isWordChar(c) (c) = 'A'\n\
isWordChar(strlens_list[i]);\n";
    const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                          strlen(token_string));
    mcc_InitialiseMacros();
    mcc_FileOpenerInitialise();
    mcc_TokenList_t *tokens = mcc_TokeniseFile(file);
    printf("Test Multiple Long List Replacement...");

    mcc_TokenList_t *output = mcc_PreprocessTokens(tokens);
    // Getting here means the replacement didn't crap out
    mcc_TokenListDelete(output);
    mcc_TokenListDelete(tokens);
    mcc_FileOpenerDelete();
    unlink(file);
    mcc_DeleteAllMacros();
    printf("ok\n");
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
    test_Implementation();
    if (false)
        test_SemiRecursiveMacroFunction();
    test_MultipleLongListReplacement();
    return 0;
}
