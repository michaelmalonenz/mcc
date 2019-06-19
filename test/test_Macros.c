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
   mcc_TokenList_t *tokens = mcc_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   printf("Testing Define...");
   mcc_TokenListStandaloneAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL);
   mcc_DeleteAllMacros();
   printf("ok\n");
}

static void test_Find(void)
{	
   mcc_TokenList_t *tokens = mcc_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   printf("Testing Find...");
   mcc_TokenListStandaloneAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL);
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
   mcc_TokenList_t *tokens = mcc_ListCreate();
   mcc_Token_t *tok = mcc_CreateToken("1", 1, TOK_NUMBER, TOK_UNSET_INDEX, 1, 1, 1);
   printf("Testing Define...");
   mcc_TokenListStandaloneAppend(tokens, tok);
   mcc_DefineMacro(MACRO_NAME, tokens, NULL);
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
   for(i = 0; i < NUM_BULK_MACROS; i++)
   {
      mcc_TokenList_t *tokenList;
      mcc_TokenListIterator_t *tokenListIter;
      const char *tempFilename = mcc_TestUtils_DumpStringToTempFile(
         test_MacroValues[i],
         strlen(test_MacroValues[i]));

      mcc_FileOpenerInitialise();

      tokenList = mcc_TokenListCreateStandalone();
      tokenListIter = mcc_TokenListStandaloneGetIterator(tokenList);
      mcc_TokeniseFile(tempFilename, tokenListIter);
      mcc_TokenListDeleteIterator(tokenListIter);

      mcc_DefineMacro(test_Macros[i], mcc_GetTokenList(), NULL);

      unlink(tempFilename);
      mcc_TokenListDeleteStandalone(tokenList);
      mcc_FileOpenerDelete();
   }
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
   return 0;
}
