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
      else if (token->tokenType == TOK_NUMBER ||
         token->tokenType == TOK_IDENTIFIER)
      {
         mcc_StackPush(operands, (uintptr_t) token);
      }
/*
      else if (token->tokenType != TOK_WHITESPACE &&
               token->tokenType != TOK_EOL)
      {
         MCC_ASSERT(FALSE);
      }
*/
      token = mcc_GetNextToken(iter);
   }

   mcc_StackDelete(operators, NULL);
   mcc_StackDelete(operands, NULL);
   //BEDMAS
   return 0;
}
