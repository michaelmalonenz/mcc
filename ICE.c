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
#include "ICE.h"
#include "stack.h"
#include "tokens.h"

typedef enum token_associativity { TOK_ASSOC_RIGHT, TOK_ASSOC_LEFT, TOK_ASSOC_UNARY } token_associativity_t;
/*
Operators
	() [] . -> expr++ expr-- 

Unary Operators	* & + - ! ~ ++expr --expr (typecast) sizeof	right-to-left
Binary Operators	* / %	left-to-right
+ -
>> <<
< > <= >=
== !=
&
^
|
&&
||
Ternary Operator	?:	right-to-left
Assignment Operators	= += -= *= /= %= >>= <<= &= ^= |=	right-to-left
Comma	,
*/

static token_associativity_t getOperatorAssociativity(const mcc_Token_t *token)
{
   if (token->tokenType == TOK_OPERATOR &&
       (token->tokenIndex == OP_BITWISE_EXCL_OR || token->tokenIndex == OP_DEREFERENCE ||
        token->tokenIndex == OP_ADDRESS_OF || token->tokenIndex == OP_NOT ||
        token->tokenIndex == OP_NEGATE || token->tokenIndex == OP_SIZEOF ||
        token->tokenIndex == OP_TERNARY_IF || token->tokenIndex == OP_TERNARY_ELSE ||
        token->tokenIndex == OP_TIMES_EQUALS || token->tokenIndex == OP_DIVIDE_EQUALS ||
        token->tokenIndex == OP_MOD_EQUALS || token->tokenIndex == OP_PLUS_EQUALS || 
        token->tokenIndex == OP_MINUS_EQUALS || token->tokenIndex == OP_L_SHIFT_EQUALS ||
        token->tokenIndex == OP_R_SHIFT_EQUALS || token->tokenIndex == OP_BITWISE_AND_EQUALS ||
        token->tokenIndex == OP_BITWISE_EXCL_OR_EQUALS || token->tokenIndex == OP_BITWISE_INCL_OR_EQUALS))
   {
      return TOK_ASSOC_RIGHT;
   }
   return TOK_ASSOC_LEFT;
}

static int getRelativeOperatorPrecedence(MCC_OPERATOR op)
{
   static const int precedents[NUM_OPERATORS] = {
      [OP_MEMBER_OF] = 1, [OP_DEREF_MEMBER_OF] = 2,
      [OP_DECREMENT] = 3, [OP_INCREMENT] = 3, [OP_DEREFERENCE] = 4,
      [OP_ADDRESS_OF] = 4, [OP_NOT] = 5, [OP_NEGATE] = 6, 
      [OP_SIZEOF] = 7, [OP_MULTIPLY] = 8, [OP_DIVIDE] = 8, [OP_MODULO] = 9,
      [OP_ADD] = 10, [OP_MINUS] = 10, [OP_R_SHIFT] = 11, [OP_L_SHIFT] = 11,
      [OP_LESS_THAN] = 12, [OP_GREATER_THAN] = 12, [OP_LESS_EQUAL] = 13, 
      [OP_GREATER_EQUAL] = 13, [OP_COMPARE_TO] = 13, [OP_NOT_EQUAL] = 14,
      [OP_BITWISE_AND] = 15, [OP_BITWISE_EXCL_OR] = 16, 
      [OP_BITWISE_INCL_OR] = 17, [OP_LOGICAL_AND] = 18, 
      [OP_LOGICAL_INCL_OR] = 19, [OP_TERNARY_IF] = 20, [OP_TERNARY_ELSE] = 20,
      [OP_EQUALS_ASSIGN] = 21, [OP_PLUS_EQUALS] = 21, [OP_MINUS_EQUALS] = 21,
      [OP_TIMES_EQUALS] = 21, [OP_DIVIDE_EQUALS] = 21, [OP_MOD_EQUALS] = 21,
      [OP_R_SHIFT_EQUALS] = 21, [OP_L_SHIFT_EQUALS] = 21,
      [OP_BITWISE_AND_EQUALS] = 21, [OP_BITWISE_EXCL_OR_EQUALS] = 21,
      [OP_BITWISE_INCL_OR_EQUALS] = 21, [OP_COMMA] = 22,
   };
   
   return precedents[op];
}

static int mcc_EvaluateRPN(mcc_Stack_t UNUSED(*input))
{
   int result;
   mcc_Stack_t *output = mcc_StackCreate();

   result = 0;

   mcc_StackDelete(output, NULL);
   return result;
}

int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter)
{
   mcc_Stack_t *output = mcc_StackCreate();
   mcc_Stack_t *operator_stack = mcc_StackCreate();
   const mcc_Token_t *token = mcc_GetNextToken(iter);
   const mcc_Token_t *temp = NULL;
   int result;
   
   while (token->tokenType != TOK_EOL)
   {
      if (token->tokenType == TOK_NUMBER)
      {
         mcc_StackPush(output, (uintptr_t) token);
      }
      else if (token->tokenType == TOK_OPERATOR)
      {
         temp = (mcc_Token_t *) mcc_StackPeek(operator_stack);
         while( temp != NULL && temp->tokenType == TOK_OPERATOR &&
                (getRelativeOperatorPrecedence(token->tokenIndex) >= 
                 getRelativeOperatorPrecedence(temp->tokenIndex)) &&
                getOperatorAssociativity(token) == TOK_ASSOC_LEFT)
         {
            mcc_StackPush(output, mcc_StackPop(operator_stack));
            temp = (mcc_Token_t *) mcc_StackPeek(operator_stack);
         }
         mcc_StackPush(operator_stack, (uintptr_t) token);
      }
      else if (token->tokenType == TOK_SYMBOL)
      {
         if (token->tokenIndex == SYM_OPEN_PAREN)
         {
            mcc_StackPush(operator_stack, (uintptr_t) token);
         }
         else if (token->tokenIndex == SYM_CLOSE_PAREN)
         {
            temp = (mcc_Token_t *) mcc_StackPop(operator_stack);
            while(!mcc_StackEmpty(operator_stack))
            {
               if (temp->tokenType == TOK_SYMBOL &&
                   temp->tokenIndex == SYM_OPEN_PAREN)
               {
                  break;
               }
               mcc_StackPush(output, (uintptr_t) temp);
               temp = (mcc_Token_t *) mcc_StackPop(operator_stack);
            }
            if (temp == NULL)
            {
               mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),
                               token->lineno,
                               "Unmatched Parentheses\n");
            }
         }
         else
         {
            mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),
                            token->lineno,
                            "Unexpected symbol in arithmetic statement: %s\n",
                            symbols[token->tokenIndex]);
         }
      }
      else
      {
#if MCC_DEBUG
         if (token->tokenType != TOK_WHITESPACE)
         {
            MCC_ASSERT(FALSE);
            mcc_DebugPrintToken(token);
         }
#endif
      }
      token = mcc_GetNextToken(iter);
   }

   while(!mcc_StackEmpty(operator_stack))
   {
      mcc_StackPush(output, mcc_StackPop(operator_stack));
   }

   mcc_StackDelete(operator_stack, NULL);

   result = mcc_EvaluateRPN(output);

   mcc_StackDelete(output, NULL);

   return result;
}

