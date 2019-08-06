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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MCC_DEBUG 1
#include "mcc.h"
#include "macro.h"
#include "tokenList.h"
#include "toolChainCommands.h"

#include "TestUtils.h"

#define MACRO_NAME "EAT_ME"

#define NUM_BULK_MACROS 10
const char *test_Macros[NUM_BULK_MACROS] = { "FIRST_MACRO", "SECOND_MACRO", "THIRD_MACRO", 
                                             "FOURTH_MACRO", "FIFTH_MACRO", "SIXTH_MACRO",
                                             "SEVENTH_MACRO", "EIGHTH_MACRO", "NINETH_MACRO",
                                             "TENTH_MACRO" };

char *test_MacroValues[NUM_BULK_MACROS] = { "1", "two", "1+2", "(FIRST_MACRO) + 3", "5", "2*3",
                                                  "7", "8", "(2*4+1)", "1+2*3+3" };

static void test_Define(void)
{
   mcc_TokenList_t *tokens = eral_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   printf("Testing Define...");
   mcc_TokenListAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL, FALSE);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

static void test_Find(void)
{	
   mcc_TokenList_t *tokens = eral_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   printf("Testing Find...");
   mcc_TokenListAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL, FALSE);
   mcc_Macro_t *result = mcc_ResolveMacro(MACRO_NAME);
   MCC_ASSERT(result != NULL);
   bool_t defined = mcc_IsMacroDefined(MACRO_NAME);
   MCC_ASSERT(defined);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

static void test_Undefine(void)
{
   printf("Testing undefine...");
   mcc_TokenList_t *tokens = eral_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   mcc_TokenListAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL, FALSE);
   mcc_UndefineMacro(MACRO_NAME);
   bool_t defined = mcc_IsMacroDefined(MACRO_NAME);
   MCC_ASSERT(!defined);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

static void test_Undefined(void)
{
   printf("Testing undefined...");
   bool_t defined = mcc_IsMacroDefined("SOME_OTHER_MACRO");
   MCC_ASSERT(!defined);
   printf("ok\n");
}

static void test_BulkMacros(void)
{
   int i;
   printf("Testing Bulk Macro Definitions...");
   mcc_FileOpenerInitialise();
   for(i = 0; i < NUM_BULK_MACROS; i++)
   {
      mcc_TokenList_t *tokenList;
      const char *tempFilename = mcc_TestUtils_DumpStringToTempFile(
         test_MacroValues[i],
         strlen(test_MacroValues[i]));

      tokenList = mcc_TokeniseFile(tempFilename);

      mcc_DefineMacro(test_Macros[i], mcc_TokenListDeepCopy(tokenList), NULL, FALSE);

      unlink(tempFilename);
      mcc_TokenListDelete(tokenList);
   }
   mcc_FileOpenerDelete();
   mcc_DeleteAllMacros();
   printf("ok\n");
}

void test_BuiltinDefine(void)
{
   printf("Testing builtins...");
   mcc_InitialiseMacros();

   mcc_Macro_t *stdcVersion = mcc_ResolveMacro("__STDC_VERSION__");
   MCC_ASSERT(stdcVersion != NULL);
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(stdcVersion->tokens);
   mcc_Token_t *version = mcc_GetNextToken(iter);
   MCC_ASSERT(version->number.number.integer_s == 199901L);

   mcc_Macro_t *stdc = mcc_ResolveMacro("__STDC__");
   MCC_ASSERT(stdc != NULL);

   mcc_TokenListDeleteIterator(iter);
   mcc_DeleteAllMacros();
   printf("ok!\n");
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

int main(void)
{
   test_Define();
   test_Find();
   test_Undefine();
   test_Undefined();
   test_BulkMacros();
   test_BuiltinDefine();
   test_VariadicMacroFunctionDefinition();
   return 0;
}
