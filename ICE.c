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
#include "ICE.h"
#include "stack.h"

int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter)
{
   mcc_Stack_t *operators = mcc_StackCreate();
   mcc_Stack_t *operands = mcc_StackCreate();
   const mcc_Token_t *token = mcc_GetNextToken(iter);
   while (token->tokenType != TOK_EOL)
   {
      if (token->tokenType == TOK_OPERATOR)
      {
         mcc_StackPush(operators, (uintptr_t) token);
      }
      else if (token->tokenType == TOK_NUMBER)
      {
         mcc_StackPush(operands, (uintptr_t) token);
      }
      token = mcc_GetNextToken(iter);
   }

   mcc_StackDelete(operators, NULL);
   mcc_StackDelete(operands, NULL);
   //BEDMAS
   return 0;
}
