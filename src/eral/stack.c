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
   eral_List_t *list;
};

eral_Stack_t *eral_StackCreate(void)
{
   eral_Stack_t *result = (eral_Stack_t *) malloc(sizeof(eral_Stack_t));
   result->list = eral_ListCreate();
   return result;
}

void eral_StackDelete(eral_Stack_t *stack, eral_NodeDestructor_fn destructorFn)
{
   eral_ListDelete(stack->list, destructorFn);
   free(stack);
}

void eral_StackPush(eral_Stack_t *stack, uintptr_t data)
{
   MCC_ASSERT(data != NULL_DATA);
   eral_ListAppendData(stack->list, data);
}

uintptr_t eral_StackPop(const eral_Stack_t *stack)
{
   return eral_ListRemoveTailData(stack->list);
}

uintptr_t eral_StackPeek(const eral_Stack_t *stack)
{
   return eral_ListPeekTailData(stack->list);
}

bool_t eral_StackEmpty(const eral_Stack_t *stack)
{
   return eral_ListEmpty(stack->list);
}

uint32_t eral_StackNumItems(const eral_Stack_t *stack)
{
   return eral_ListGetLength(stack->list);
}

eral_Stack_t *eral_StackReverse(eral_Stack_t *stack)
{
   eral_Stack_t *result = eral_StackCreate();
   while(!eral_StackEmpty(stack))
   {
      eral_StackPush(result, eral_StackPop(stack));
   }
   eral_StackDelete(stack, NULL);
   return result;
}

#if MCC_DEBUG
void eral_DebugPrintStack(const eral_Stack_t *stack, stackItemPrinter_t itemPrinterFn)
{
   eral_ListIterator_t *iter = eral_ListGetIterator(stack->list);
   uintptr_t item = eral_ListGetPrevData(iter);
   printf("Number of Stack Items: %d\n", eral_ListGetLength(stack->list));
   while(item != NULL_DATA)
   {
      itemPrinterFn(item);
      item = eral_ListGetPrevData(iter);
   }
   eral_ListDeleteIterator(iter);
}
#endif
