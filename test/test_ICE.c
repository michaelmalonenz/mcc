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
#include "TestUtils.h"
#include "toolChainCommands.h"
#include "tokenList.h"
#include "ICE.h"

const char *ice_is_zero = "10 * (1 + 1 - 2)\n";
const char *ice_is_nonzero_twenty = "10 * (1 + 3 - 2)\n";
static const char *ice_shunting_yard_wiki_example = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3\n";

static void test_Implementation(const char *token_string, int expected_result)
{
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   iter = mcc_TokenListGetIterator();
   MCC_ASSERT(mcc_ICE_EvaluateTokenString(iter) == expected_result);
   mcc_TokenListDeleteIterator(iter);

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
}

int main(void)
{
   test_Implementation(ice_shunting_yard_wiki_example, 0);
   test_Implementation(ice_is_zero, 0);
   test_Implementation(ice_is_nonzero_twenty, 20);

   return EXIT_SUCCESS;
}
