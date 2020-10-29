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
#include <unistd.h>
#include <stdlib.h>

#include "mcc.h"
#include "liberal.h"
#include "TestUtils.h"
#include "toolChainCommands.h"
#include "tokenList.h"
#include "ICE.h"

static const char *ice_is_zero = "10 * (1 + 1 - 2)";
static const char *ice_is_nonzero_twenty = "10 * (1 + 3 - 2)";
static const char *ice_shunting_yard_wiki_example = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3";
static const char *simple_logical_and = "1 && 1";
static const char *simple_logical_or = "1 || 0";
static const char *simple_logical_not = "!0";
static const char *simple_greater_than = "4 > 3";
static const char *simple_less_than = "3 < 8";
static const char *simple_greater_equal = "4 >= 4";
static const char *simple_less_equal = "3 <= 3";
static const char *complex_precedence = "3 <= 3 && 8 > 4";
static const char *logical_precedence = "0 || 1 && 0 || 1";
static const char *simple_equality = "3 == 3";
static const char *ternary_if_if = "3 < 4 ? 2 : 42";
static const char *ternary_if_else = "3 > 4 ? 2 : 42";
static const char *complex_ternary = "1 && (2 ? (3 && 4) : (5 || 6))";
static const char *complex_ternary_with_implicit_precedence = "0 ? 3 < 4 : 4 < 3";

static void test_Implementation(const char *token_string, int expected_result)
{
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter;
   mcc_Token_t *actual_result;
   mcc_FileOpenerInitialise();
   mcc_TokenList_t *tokens = mcc_TokeniseFile(file);

   printf("About to evaluate: '%s'\n", token_string);
   iter = mcc_TokenListGetIterator(tokens);
   if (mcc_GetNextToken(iter)->tokenType == TOK_WHITESPACE)
         (void)mcc_GetNextToken(iter);
   mcc_AST_t *tree = mcc_ParseExpression(iter);
   actual_result = mcc_ICE_EvaluateAST(tree);
   printf("Token string: %s Expected: %d, got: %d\n", 
          token_string, expected_result, actual_result->number.number.integer_s);
   MCC_ASSERT(actual_result->number.number.integer_s == expected_result);
   mcc_TokenListDeleteIterator(iter);
   mcc_DeleteToken((uintptr_t)actual_result);

   mcc_TokenListDelete(tokens);
   mcc_FileOpenerDelete();
   unlink(file);
}

int main(void)
{
   test_Implementation(ice_is_nonzero_twenty, 20);
   test_Implementation(ice_shunting_yard_wiki_example, 0);
   test_Implementation(ice_is_zero, 0);
   test_Implementation(simple_logical_and, 1);
   test_Implementation(simple_logical_or, 1);
   test_Implementation(simple_logical_not, 1);
   test_Implementation(simple_greater_than, 1);
   test_Implementation(simple_less_than, 1);
   test_Implementation(simple_greater_equal, 1);
   test_Implementation(simple_less_equal, 1);
   test_Implementation(complex_precedence, 1);
   test_Implementation(logical_precedence, 1);
   test_Implementation(simple_equality, 1);
   test_Implementation(ternary_if_if, 2);
   test_Implementation(ternary_if_else, 42);
   test_Implementation(complex_ternary_with_implicit_precedence, 0);
   test_Implementation(complex_ternary, 1);
   return EXIT_SUCCESS;
}
