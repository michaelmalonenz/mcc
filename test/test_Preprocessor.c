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


/* To test this correctly, I will either have to do only the function
 * (static) declarations at the top of the file, having them automatic
 * when they're defined. And, of course give them external linkage when
 * MCC_DEBUG is defined.  I might ask John Carter whether he thinks this 
 * is even vaguely reasonable.  I suspect he'll give an answer similar to
 * "Use your own judgement" :)
 */

static void test_NestedIf(void)
{
   const char *token_string  = "#if SOME_MACRO\n#if SOME_OTHER_MACRO\n#define TEST_MACRO 10\n#else\n#define TEST_MACRO 42\n#endif\n#endif\n";
   const char *file = mcc_TestUtils_DumpStringToTempFile(token_string,
                                                         strlen(token_string));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   //should assert that we indeed got the right symbol defined.
   mcc_Macro_t *macro = mcc_ResolveMacro("TEST_MACRO");
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(strncmp(macro->value, "42", 2));

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   unlink(file);
}


int main(int UNUSED(argc), char UNUSED(**argv))
{
   test_NestedIf();
   return 0;
}
