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
#include <stdlib.h>

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

void mcc_StackDelete(mcc_Stack_t *stack, mcc_NodeDestructor_fn destructorFn);

void mcc_StackPush(mcc_Stack_t *stack, uintptr_t data)
{
   mcc_ListAppendData(stack->list, data);
}

uintptr_t mcc_StackPop(void)
{
   return (uintptr_t) 0;
}

uintptr_t mcc_StackPeek(void)
{
   return (uintptr_t) 0;
}

