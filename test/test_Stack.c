/**
 Copyright (c) 2013, Michael Malone
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
#include <stdio.h>
#include <stdlib.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"
#include "stack.h"

#define NUM_TEST_DATA 10
static int basicTestData[NUM_TEST_DATA] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

static void stackItemPrinter(uintptr_t item)
{
   printf("Stack Item: %lu\n", item);
}

int main(void)
{
   int i;
   mcc_Stack_t *stack = mcc_StackCreate();

   for (i = 0; i < NUM_TEST_DATA; i++)
   {
      printf("Pushing %d\n", basicTestData[i]);
      mcc_StackPush(stack, basicTestData[i]);
   }
   
   mcc_DebugPrintStack(stack, stackItemPrinter);

   for (i = NUM_TEST_DATA-1; i >= 0; i--)
   {
      int result = mcc_StackPop(stack);
      printf("Popped %d\n", result);
      MCC_ASSERT(result == basicTestData[i]);
   }

   mcc_StackDelete(stack, NULL);

   return EXIT_SUCCESS;
}
