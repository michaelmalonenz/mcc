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
#include "list.h"

typedef struct stack mcc_Stack_t;

mcc_Stack_t *mcc_StackCreate(void);

void mcc_StackDelete(mcc_Stack_t *stack, mcc_NodeDestructor_fn destructorFn);

void mcc_StackPush(mcc_Stack_t *stack, uintptr_t data);

uintptr_t mcc_StackPop(mcc_Stack_t *stack);

uintptr_t mcc_StackPeek(mcc_Stack_t *stack);

bool_t mcc_StackEmpty(mcc_Stack_t *stack);
