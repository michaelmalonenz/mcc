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
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include "ICE.h"
#include "stack.h"
#include "tokens.h"


/*
static int getRelativeOperatorPrecedence(MCC_OPERATOR op)
{
   static const int priorities[NUM_OPERATORS] = {
      [OP_DECREMENT] = 1, [OP_INCREMENT] = 1, 
      [OP_TIMES_EQUALS] = 2, [OP_DIVIDE_EQUALS] = 2,
      [OP_MOD_EQUALS] = 2, [OP_PLUS_EQUALS] = 2, 
      [OP_MINUS_EQUALS] = 2, [OP_L_SHIFT_EQUALS] = 2,
      [OP_R_SHIFT_EQUALS] = 2, [OP_BITWISE_AND_EQUALS] = 2,
      [OP_BITWISE_EXCL_OR_EQUALS] = 3, [OP_BITWISE_INCL_OR_EQUALS] = 4,
      [OP_COMPARE_TO] = 5, [OP_NOT_EQUAL] = 6, [OP_GREATER_THAN] = 7, 
      [OP_LESS_THAN] = 8, [OP_GREATER_EQUAL] = 9, [OP_LESS_EQUAL] = 10, 
      [OP_LOGICAL_AND] = 11, [OP_LOGICAL_EXCL_OR] = 12,
      [OP_LOGICAL_INCL_OR] = 13, [OP_NOT] = 14, [OP_BITWISE_AND] = 15, 
      [OP_BITWISE_INCL_OR] = 16, [OP_BITWISE_EXCL_OR] = 17, [OP_L_SHIFT] = 18,
      [OP_R_SHIFT] = 19, [OP_NEGATE] = 20, [OP_SIZEOF] = 21, [OP_ADDRESS_OF] = 22, 
      [OP_TERNARY_IF] = 23, [OP_TERNARY_ELSE] = 24, [OP_MEMBER_OF] = 25,
      [OP_DEREF_MEMBER_OF] = 26, [OP_EQUALS_ASSIGN] = 27, [OP_COMMA] = 28,
      [OP_ADD] = 29, [OP_MINUS] = 30, [OP_DIVIDE] = 31, [OP_MULTIPLY] = 32,
      [OP_MODULO] = 33
   };
   
   return priorities[op];
}


static bool_t shouldPlaceOnOutputQueue(const mcc_Stack_t *operators, const mcc_Token_t *current)
{
   if (current->tokenType == TOK_NUMBER || current->tokenType == TOK_IDENTIFIER ||
       (current->tokenType == TOK_OPERATOR && mcc_StackEmpty(operators)))
   {
      return TRUE;
   }
   if (current->tokenType == TOK_OPERATOR)
   { 
      mcc_Token_t *top_operator = (mcc_Token_t *) mcc_StackPeek(operators);
      return ((getRelativeOperatorPrecedence(top_operator->tokenType) < 
               getRelativeOperatorPrecedence(current->tokenType)));
   }
   MCC_ASSERT(current->tokenType != TOK_OPERATOR); //we have something in an arithmetic expression I wasn't expecting
   return FALSE;
}
*/

int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter)
{
   mcc_Stack_t *operators = mcc_StackCreate();
   mcc_Stack_t *output = mcc_StackCreate();
   const mcc_Token_t *token = mcc_GetNextToken(iter);
   const mcc_Token_t *temp = NULL;
/*
While there are tokens to be read:
Read a token.
If the token is a number, then add it to the output queue.
If the token is a function token, then push it onto the stack.
If the token is a function argument separator (e.g., a comma):
Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue. If no left parentheses are encountered, either the separator was misplaced or parentheses were mismatched.
If the token is an operator, o1, then:
while there is an operator token, o2, at the top of the stack, and
either o1 is left-associative and its precedence is less than or equal to that of o2,
or o1 has precedence less than that of o2,
pop o2 off the stack, onto the output queue;
push o1 onto the stack.
If the token is a left parenthesis, then push it onto the stack.
If the token is a right parenthesis:
Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
Pop the left parenthesis from the stack, but not onto the output queue.
If the token at the top of the stack is a function token, pop it onto the output queue.
If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
When there are no more tokens to read:
While there are still operator tokens in the stack:
If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
Pop the operator onto the output queue.
Exit.
*/
   
   while (token->tokenType != TOK_EOL)
   {
      if (token->tokenType == TOK_NUMBER)
      {
         mcc_StackPush(output, (uintptr_t) token);
      }
      else if (token->tokenType == TOK_OPERATOR)
      {
         temp = (mcc_Token_t *) mcc_StackPeek(operators);
         if (temp != NULL && )
         {
            mcc_StackPush(operators, (uintptr_t) token);
         }
      }
      else
      {
         //something's gone wrong?
      }
      token = mcc_GetNextToken(iter);
   }

   mcc_StackDelete(operators, NULL);
   mcc_StackDelete(output, NULL);
   return 0;
}

