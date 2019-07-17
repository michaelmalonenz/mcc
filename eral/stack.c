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
#include <stdlib.h>

#include "mcc.h"
#include "stack.h"
#include "list.h"

struct stack {
   mcc_List_t *list;
};

mcc_Stack_t *mcc_StackCreate(void)
{
   mcc_Stack_t *result = (mcc_Stack_t *) malloc(sizeof(mcc_Stack_t));
   result->list = mcc_ListCreate();
   return result;
}

void mcc_StackDelete(mcc_Stack_t *stack, mcc_NodeDestructor_fn destructorFn)
{
   mcc_ListDelete(stack->list, destructorFn);
   free(stack);
}

void mcc_StackPush(mcc_Stack_t *stack, uintptr_t data)
{
   MCC_ASSERT(data != NULL_DATA);
   mcc_ListAppendData(stack->list, data);
}

uintptr_t mcc_StackPop(const mcc_Stack_t *stack)
{
   return mcc_ListRemoveTailData(stack->list);
}

uintptr_t mcc_StackPeek(const mcc_Stack_t *stack)
{
   return mcc_ListPeekTailData(stack->list);
}

bool_t mcc_StackEmpty(const mcc_Stack_t *stack)
{
   return mcc_ListEmpty(stack->list);
}

uint32_t mcc_StackNumItems(const mcc_Stack_t *stack)
{
   return mcc_ListGetLength(stack->list);
}

mcc_Stack_t *mcc_StackReverse(mcc_Stack_t *stack)
{
   mcc_Stack_t *result = mcc_StackCreate();
   while(!mcc_StackEmpty(stack))
   {
      mcc_StackPush(result, mcc_StackPop(stack));
   }
   mcc_StackDelete(stack, NULL);
   return result;
}

#if MCC_DEBUG
void mcc_DebugPrintStack(const mcc_Stack_t *stack, stackItemPrinter_t itemPrinterFn)
{
   mcc_ListIterator_t *iter = mcc_ListGetIterator(stack->list);
   uintptr_t item = mcc_ListGetPrevData(iter);
   printf("Number of Stack Items: %d\n", mcc_ListGetLength(stack->list));
   while(item != NULL_DATA)
   {
      itemPrinterFn(item);
      item = mcc_ListGetPrevData(iter);
   }
   mcc_ListDeleteIterator(iter);
}
#endif