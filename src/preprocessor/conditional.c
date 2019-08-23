/**
 Copyright (c) 2019, Michael Malone
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
#include <string.h>

#include "preprocessor_private.h"
#include "ICE.h"
#include "macro.h"

static void conditionalInnerImpl(preprocessor_t *preprocessor, bool_t initialConditionTrue, bool_t ignore);
static void handleIfDefInner(preprocessor_t *preprocessor, bool_t ignore);
static void handleIfNDefInner(preprocessor_t *preprocessor, bool_t ignore);
static void handleIfInner(preprocessor_t *preprocessor, bool_t ignore);
static bool_t handleElIfInner(preprocessor_t *preprocessor, bool_t ignore);


static eral_List_t *parseConditionalExpression(preprocessor_t *preprocessor, bool_t ignore)
{
   mcc_TokenList_t *list = mcc_TokenListCreate();
   getToken(preprocessor);
   while (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      if (ignore)
      {
         getToken(preprocessor);
         continue;
      }
      if (preprocessor->currentToken->tokenType == TOK_IDENTIFIER)
      {
         if (strncmp(preprocessor->currentToken->text,
             "defined", strlen(preprocessor->currentToken->text)) == 0)
         {
            bool_t expectClosingParen = FALSE;
            getToken(preprocessor);
            maybeGetWhiteSpaceToken(preprocessor);
            if (preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                preprocessor->currentToken->tokenIndex == SYM_OPEN_PAREN)
            {
               expectClosingParen = TRUE;
               getToken(preprocessor);
               maybeGetWhiteSpaceToken(preprocessor);
            }
            mcc_ExpectTokenType(preprocessor->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
            bool_t defined = mcc_IsMacroDefined(preprocessor->currentToken->text);
            mcc_Number_t number;
            number.number.integer_s = (int) defined;
            number.numberType = SIGNED_INT;
            mcc_Token_t *token = mcc_CreateNumberToken(
               &number,
               preprocessor->currentToken->line_index,
               preprocessor->currentToken->lineno,
               preprocessor->currentToken->fileno);
            mcc_TokenListAppend(list, token);
            if (expectClosingParen) {
               getToken(preprocessor);
               maybeGetWhiteSpaceToken(preprocessor);
               mcc_ExpectTokenType(preprocessor->currentToken, TOK_SYMBOL, SYM_CLOSE_PAREN);
            }
         }
         else
         {
            mcc_TokenList_t *macroTokens = resolveMacroTokens(preprocessor, preprocessor->currentToken->text);
            if (macroTokens != NULL)
            {
               mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(macroTokens);
               mcc_Token_t *token = mcc_GetNextToken(iter);
               while (token != NULL)
               {
                  mcc_TokenListAppend(list, mcc_CopyToken(token));
                  token = mcc_GetNextToken(iter);
               }
               mcc_TokenListDeleteIterator(iter);
               mcc_TokenListDelete(macroTokens);
            }
            else
            {
               // This is super dumb.  I think I have to pretend this is 0 because of
               // cases like (defined SOME_MACRO && SOME_MACRO || SOME_OTHER_MACRO)
               mcc_Number_t number;
               number.number.integer_s = 0;
               number.numberType = SIGNED_INT;
               mcc_Token_t *undefinedMacroToken = mcc_CreateNumberToken(&number,
                  preprocessor->currentToken->line_index, preprocessor->currentToken->lineno, preprocessor->currentToken->fileno);
               mcc_TokenListAppend(list, undefinedMacroToken);
            }
         }
      }
      else
      {
         mcc_TokenListAppend(list, mcc_CopyToken(preprocessor->currentToken));
      }
      getToken(preprocessor);
   }
   return list;
}

static void conditionalInnerImpl(
   preprocessor_t *preprocessor,
   bool_t initialConditionTrue,
   bool_t ignore)
{
   bool_t processMacro = initialConditionTrue && !ignore;
   bool_t handled = FALSE;
   getToken(preprocessor);
   while (TRUE)
   {
      if (preprocessor->currentToken == NULL)
      {
         mcc_Error("Unexpected End Of File\n");
      }
      if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
          preprocessor->currentToken->tokenIndex == PP_ENDIF)
      {
         break;
      }
      else if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
               preprocessor->currentToken->tokenIndex == PP_IFDEF)
      {
         handleIfDefInner(preprocessor, !processMacro);
      }
      else if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
               preprocessor->currentToken->tokenIndex == PP_IFNDEF)
      {
         handleIfNDefInner(preprocessor, !processMacro);
      }
      else if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
               preprocessor->currentToken->tokenIndex == PP_IF)
      {
         handleIfInner(preprocessor, !processMacro);
      }
      else if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
               preprocessor->currentToken->tokenIndex == PP_ELIF)
      {
         handled = handled || processMacro;
         processMacro = handleElIfInner(preprocessor, ignore || handled);
      }
      else if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE &&
               preprocessor->currentToken->tokenIndex == PP_ELSE)
      {
         handled = handled || processMacro;
         processMacro = !processMacro && !ignore;
      }
      else if (processMacro && !handled)
      {
         MCC_ASSERT(!ignore);
         if (preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE)
         {
            handlePreprocessorDirective(preprocessor);
         }
         else
         {
            emitToken(preprocessor);
         }
      }
      getToken(preprocessor);
   }
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_PP_DIRECTIVE, PP_ENDIF);
}

void handleIfdef(preprocessor_t *preprocessor)
{
   handleIfDefInner(preprocessor, FALSE);
}
static void handleIfDefInner(preprocessor_t *preprocessor, bool_t ignore)
{
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(preprocessor, mcc_IsMacroDefined(preprocessor->currentToken->text), ignore);
}

void handleIfndef(preprocessor_t *preprocessor)
{
   handleIfNDefInner(preprocessor, FALSE);
}
static void handleIfNDefInner(preprocessor_t *preprocessor, bool_t ignore)
{
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(preprocessor, !mcc_IsMacroDefined(preprocessor->currentToken->text), ignore);
}

void handleIf(preprocessor_t *preprocessor)
{
   handleIfInner(preprocessor, FALSE);
}
static void handleIfInner(preprocessor_t *preprocessor, bool_t ignore)
{
   mcc_TokenList_t *list = parseConditionalExpression(preprocessor, ignore);
   if (!ignore)
   {
      mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(list);
      if (mcc_GetNextToken(iter)->tokenType == TOK_WHITESPACE)
         (void)mcc_GetNextToken(iter);
      mcc_AST_t *tree = mcc_ParseExpression(iter);
      mcc_Token_t *result = mcc_ICE_EvaluateAST(tree);
      conditionalInnerImpl(preprocessor, result->number.number.integer_s, ignore);
      mcc_DeleteToken((uintptr_t) result);
      mcc_TokenListDeleteIterator(iter);
   }
   else
   {
      conditionalInnerImpl(preprocessor, FALSE, ignore);
   }
   mcc_TokenListDelete(list);
}

static bool_t handleElIfInner(preprocessor_t *preprocessor, bool_t ignore)
{
   mcc_TokenList_t *list = parseConditionalExpression(preprocessor, ignore);
   bool_t result;
   if (!ignore)
   {
      mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(list);
      if (mcc_GetNextToken(iter)->tokenType == TOK_WHITESPACE)
         (void)mcc_GetNextToken(iter);
      mcc_AST_t *tree = mcc_ParseExpression(iter);
      mcc_Token_t *tok = mcc_ICE_EvaluateAST(tree);
      result = tok->number.number.integer_s;
      mcc_DeleteToken((uintptr_t) tok);
      mcc_TokenListDeleteIterator(iter);
   }
   mcc_TokenListDelete(list);
   return result;
}

void handleEndif(preprocessor_t *preprocessor)
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                     preprocessor->currentToken->lineno,
                     preprocessor->currentToken->line_index,
                     "endif without if\n");
}

void handleElse(preprocessor_t *preprocessor)
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                     preprocessor->currentToken->lineno,
                     preprocessor->currentToken->line_index,
                     "else without if\n");
}

void handleElif(preprocessor_t *preprocessor)
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                     preprocessor->currentToken->lineno,
                     preprocessor->currentToken->line_index,
                     "elif without if\n");
}
