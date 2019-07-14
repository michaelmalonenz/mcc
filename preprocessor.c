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
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "macro.h"
#include "tokens.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "ICE.h"
#include "list.h"

typedef void (preprocessorDirectiveHandler_t)(void);

static void handleInclude(void);
static void handleDefine(void);
static void handleIfdef(void);
static void handleIfndef(void);
static void handleIf(void);
static void handleEndif(void);
static void handleElse(void);
static void handleElif(void);
static void handleUndef(void);
static void handleError(void);
static void handlePragma(void);
static void handleJoin(void);
static void handleWarning(void);
static void handleStringify(void);
static mcc_TokenList_t *handleMacroFunction(mcc_Macro_t *macro);
static mcc_TokenList_t *handleMacroReplacement(mcc_Macro_t *macro);
static void conditionalInnerImpl(bool_t initialConditionTrue, bool_t ignore);
static void handleIfDefInner(bool_t ignore);
static void handleIfNDefInner(bool_t ignore);
static void handleIfInner(bool_t ignore);
static bool_t handleElIfInner(bool_t ignore);


static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = {
   &handleInclude, &handleDefine, &handleIfdef,
   &handleIfndef, &handleIf, &handleEndif,
   &handleElse, &handleElif, &handleUndef,
   &handleError, &handlePragma, &handleJoin,
   &handleWarning, &handleStringify
};

static mcc_TokenListIterator_t *tokenListIter;
static const mcc_Token_t *currentToken;
static mcc_List_t *output;

static mcc_TokenList_t *resolveMacroTokens(const char *macroText)
{
   mcc_Macro_t *macro = mcc_ResolveMacro(macroText);
   if (macro == NULL)
   {
      return NULL;
   }

   if (macro->is_function)
   {
      return handleMacroFunction(macro);
   }
   else
   {
      return handleMacroReplacement(macro);
   }
}

static void emitToken(void)
{
   if (currentToken->tokenType == TOK_IDENTIFIER)
   {
      mcc_TokenList_t *macroTokens = resolveMacroTokens(currentToken->text);
      if (macroTokens == NULL)
      {
         mcc_TokenListStandaloneAppend(output, mcc_CopyToken(currentToken));
      }
      else
      {
         mcc_TokenListIterator_t *macroTokensIter = mcc_TokenListStandaloneGetIterator(macroTokens);
         mcc_Token_t *token = mcc_GetNextToken(macroTokensIter);
         while (token != NULL)
         {
            mcc_TokenListStandaloneAppend(output, mcc_CopyToken(token));
            token = mcc_GetNextToken(macroTokensIter);
         }
         mcc_TokenListDeleteIterator(macroTokensIter);
         mcc_TokenListDeleteStandalone(macroTokens);
      }
   }
   else
   {
      mcc_TokenListStandaloneAppend(output, mcc_CopyToken(currentToken));
   }
}

static void getToken(void)
{
   currentToken = mcc_GetNextToken(tokenListIter);
}

static void maybeGetWhitespaceToken(void)
{
   if (currentToken->tokenType == TOK_WHITESPACE)
   {
      getToken();
   }
}

static void handlePreprocessorDirective()
{
   MCC_ASSERT(currentToken->tokenType == TOK_PP_DIRECTIVE);
   ppHandlers[currentToken->tokenIndex]();
}

mcc_List_t *mcc_PreprocessCurrentTokens(void)
{
   output = mcc_ListCreate();
   tokenListIter = mcc_TokenListGetIterator();
   getToken();

   while(currentToken != NULL)
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         handlePreprocessorDirective();
      }
      else
      {
         emitToken();
      }
      getToken();
   }
   mcc_TokenListDeleteIterator(tokenListIter);
   return output;
}

static void handleInclude()
{
   mcc_TokenListIterator_t *incIter;
   char *include_path;
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
#if MCC_DEBUG
   printf("Including file from %s:%d: %s\n",
      mcc_ResolveFileNameFromNumber(currentToken->fileno),
      currentToken->lineno,
      currentToken->text);
#endif
   if (currentToken->tokenType == TOK_LOCAL_FILE_INC)
   {
      include_path = mcc_FindLocalInclude(currentToken->text);
   }
   else if (currentToken->tokenType == TOK_SYS_FILE_INC)
   {
      include_path = mcc_FindSystemInclude(currentToken->text);
   }
   else
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                        currentToken->lineno,
                        currentToken->line_index,
                        "Expected a filename to include, got '%s'\n",
                        currentToken->text);
   }
   if (include_path == NULL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                        currentToken->lineno,
                        currentToken->line_index,
                        "Couldn't locate file '%s' for inclusion\n",
                        currentToken->text);
   }

   while (currentToken->tokenType != TOK_EOL)
   {
      getToken();
   }
   incIter = mcc_TokenListCopyIterator(tokenListIter);
   mcc_TokeniseFile(include_path, incIter);
   free(include_path);
   mcc_TokenListDeleteIterator(incIter);
}

static void handleDefine()
{
   const char *macro_identifier;
   mcc_TokenList_t *tokens = mcc_TokenListCreateStandalone();
   mcc_TokenList_t *arguments = NULL;
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   macro_identifier = currentToken->text;
   getToken();
   if (currentToken->tokenType == TOK_SYMBOL &&
       currentToken->tokenIndex == SYM_OPEN_PAREN)
   {
      arguments = mcc_TokenListCreateStandalone();
      getToken();
      while (currentToken->tokenType != TOK_SYMBOL &&
             currentToken->tokenIndex != SYM_CLOSE_PAREN)
      {
         if (currentToken->tokenType == TOK_EOL)
         {
            mcc_PrettyError(
               mcc_ResolveFileNameFromNumber(currentToken->fileno),
               currentToken->lineno,
               currentToken->line_index,
               "Unclosed parentheses in function macro '%s'\n",
               macro_identifier);
         }
         if ((currentToken->tokenType == TOK_OPERATOR &&
              currentToken->tokenIndex == OP_COMMA) ||
             currentToken->tokenType == TOK_WHITESPACE)
         {
            // Do nothing.
            // This is fine and normal and expected, but I don't care.
            // I should attempt to deal with two commas in a row, but
            // other than that, it's fine
         }
         if (currentToken->tokenType == TOK_IDENTIFIER)
         {
            mcc_TokenListStandaloneAppend(arguments, mcc_CopyToken(currentToken));
         }
         getToken();
      }
   }
   if (currentToken->tokenType != TOK_EOL)
   {
      getToken();
      maybeGetWhitespaceToken();
   }
   while (currentToken->tokenType != TOK_EOL)
   {
      mcc_TokenListStandaloneAppend(tokens, mcc_CopyToken(currentToken));
      getToken();
   }
   mcc_DefineMacro(macro_identifier, tokens, arguments);
}

static void handleUndef()
{
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   mcc_UndefineMacro(currentToken->text);
   getToken();
   maybeGetWhitespaceToken();
   if (currentToken->tokenType != TOK_EOL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      currentToken->line_index,
                      "Unexpected characters after #undef '%s'\n",
                      currentToken->text);
   }
}

static void handleError()
{
   const mcc_Token_t *first = currentToken;
   mcc_StringBuffer_t *buffer = mcc_CreateStringBuffer();
   getToken();
   while (currentToken->tokenType != TOK_EOL)
   {
      mcc_StringBufferAppendString(buffer, currentToken->text);
      getToken();
   }
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(first->fileno),
                   first->lineno,
                   first->line_index,
                   "Error: %s\n", mcc_StringBufferGetString(buffer));
}

static mcc_List_t *parseConditionalExpression(bool_t ignore)
{
   mcc_TokenList_t *list = mcc_TokenListCreateStandalone();
   getToken();
   while (currentToken->tokenType != TOK_EOL)
   {
      if (ignore)
      {
         getToken();
         continue;
      }
      if (currentToken->tokenType == TOK_IDENTIFIER)
      {
         if (strncmp(currentToken->text, "defined", strlen(currentToken->text)) == 0)
         {
            getToken();
            maybeGetWhitespaceToken();
            mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
            bool_t defined = mcc_IsMacroDefined(currentToken->text);
            mcc_Number_t number;
            number.number.integer_s = (int) defined;
            number.numberType = SIGNED_INT;
            mcc_Token_t *token = mcc_CreateNumberToken(
               &number,
               currentToken->line_index,
               currentToken->lineno,
               currentToken->fileno);
            mcc_TokenListStandaloneAppend(list, token);
         }
         else
         {
            mcc_TokenList_t *macroTokens = resolveMacroTokens(currentToken->text);
            if (macroTokens != NULL)
            {
               mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(macroTokens);
               mcc_Token_t *token = mcc_GetNextToken(iter);
               while (token != NULL)
               {
                  mcc_TokenListStandaloneAppend(list, mcc_CopyToken(token));
                  token = mcc_GetNextToken(iter);
               }
               mcc_TokenListDeleteIterator(iter);
               mcc_TokenListDeleteStandalone(macroTokens);
            }
            else
            {
               // This is super dumb.  I think I have to pretend this is 0 because of
               // cases like (defined SOME_MACRO && SOME_MACRO || SOME_OTHER_MACRO)
               mcc_Number_t number;
               number.number.integer_s = 0;
               number.numberType = SIGNED_INT;
               mcc_Token_t *undefinedMacroToken = mcc_CreateNumberToken(&number,
                  currentToken->line_index, currentToken->lineno, currentToken->fileno);
               mcc_TokenListStandaloneAppend(list, undefinedMacroToken);
            }
         }
      }
      else
      {
         mcc_TokenListStandaloneAppend(list, mcc_CopyToken(currentToken));
      }
      getToken();
   }
   return list;
}

static void conditionalInnerImpl(bool_t initialConditionTrue, bool_t ignore)
{
   bool_t processMacro = initialConditionTrue && !ignore;
   bool_t handled = FALSE;

   getToken();
   while (TRUE)
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
          currentToken->tokenIndex == PP_ENDIF)
      {
         break;
      }
      else if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
               currentToken->tokenIndex == PP_IFDEF)
      {
         handleIfDefInner(!processMacro);
      }
      else if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
               currentToken->tokenIndex == PP_IFNDEF)
      {
         handleIfNDefInner(!processMacro);
      }
      else if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
               currentToken->tokenIndex == PP_IF)
      {
         handleIfInner(!processMacro);
      }
      else if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
               currentToken->tokenIndex == PP_ELIF)
      {
         processMacro = handleElIfInner(!processMacro);
      }
      else if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
               currentToken->tokenIndex == PP_ELSE)
      {
         handled = handled || processMacro;
         processMacro = !processMacro && !ignore;
      }
      else if (processMacro && !handled)
      {
         if (currentToken->tokenType == TOK_PP_DIRECTIVE)
         {
            handlePreprocessorDirective(currentToken, tokenListIter);
         }
         else
         {
            MCC_ASSERT(!ignore);
            emitToken();
         }
      }
      getToken();
   }
   mcc_ExpectTokenType(currentToken, TOK_PP_DIRECTIVE, PP_ENDIF);
}

static void handleIfdef()
{
   handleIfDefInner(FALSE);
}
static void handleIfDefInner(bool_t ignore)
{
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(mcc_IsMacroDefined(currentToken->text), ignore);
}

static void handleIfndef()
{
   handleIfNDefInner(FALSE);
}
static void handleIfNDefInner(bool_t ignore)
{
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(!mcc_IsMacroDefined(currentToken->text), ignore);
}

static void handleIf()
{
   handleIfInner(FALSE);
}
static void handleIfInner(bool_t ignore)
{
   mcc_TokenList_t *list = parseConditionalExpression(ignore);
   if (!ignore)
   {
      mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(list);
      mcc_Token_t *result = mcc_ICE_EvaluateTokenString(iter);
      conditionalInnerImpl(result->number.number.integer_s, ignore);
      mcc_DeleteToken((uintptr_t) result);
      mcc_TokenListDeleteIterator(iter);
   }
   else
   {
      conditionalInnerImpl(FALSE, ignore);
   }
   mcc_TokenListDeleteStandalone(list);
}

static bool_t handleElIfInner(bool_t ignore)
{
   mcc_TokenList_t *list = parseConditionalExpression(ignore);
   bool_t result;
   if (!ignore)
   {
      mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(list);
      mcc_Token_t *tok = mcc_ICE_EvaluateTokenString(iter);
      result = tok->number.number.integer_s;
      mcc_DeleteToken((uintptr_t) tok);
      mcc_TokenListDeleteIterator(iter);
   }
   mcc_TokenListDeleteStandalone(list);
   return result;
}

static void handleEndif()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     currentToken->line_index,
                     "endif without if\n");
}

static void handleElse()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     currentToken->line_index,
                     "else without if\n");
}

static void handleElif()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     currentToken->line_index,
                     "elif without if\n");
}

static void handleJoin()
{
   MCC_ASSERT(FALSE);
}

//What shall I do with #pragmas???
static void handlePragma()
{
   MCC_ASSERT(FALSE);
}

static void handleWarning()
{
   const mcc_Token_t *first = currentToken;
   mcc_StringBuffer_t *buffer = mcc_CreateStringBuffer();
   getToken();
   while (currentToken->tokenType != TOK_EOL)
   {
      mcc_StringBufferAppendString(buffer, currentToken->text);
      getToken();
   }
   printf("%s:%d Warning: %s\n",
      mcc_ResolveFileNameFromNumber(first->fileno),
      first->lineno,
      mcc_StringBufferGetString(buffer));
   mcc_DeleteStringBuffer(buffer);
}

static mcc_TokenList_t *handleMacroReplacement(mcc_Macro_t *macro)
{
   mcc_TokenList_t *result = mcc_TokenListCreateStandalone();
   mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(macro->tokens);
   mcc_Token_t *macroToken = mcc_GetNextToken(iter);
   while (macroToken != NULL)
   {
      mcc_TokenListStandaloneAppend(result, mcc_CopyToken(macroToken));
      macroToken = mcc_GetNextToken(iter);
   }
   mcc_TokenListDeleteIterator(iter);
   return result;
}

mcc_List_t *replaceMacroTokens(mcc_Macro_t *macro, mcc_List_t *parameters)
{
   mcc_List_t *functionTokens = mcc_TokenListDeepCopy(macro->tokens);
   mcc_TokenListIterator_t *parametersIter = mcc_TokenListStandaloneGetIterator(parameters);
   mcc_MacroParameter_t *param = (mcc_MacroParameter_t *) mcc_ListGetNextData(parametersIter);
   while (param != NULL)
   {
      mcc_TokenListIterator_t *tokensIter = mcc_TokenListStandaloneGetIterator(functionTokens);
      mcc_Token_t *functionToken = mcc_GetNextToken(tokensIter);
      while (functionToken != NULL)
      {
         if (functionToken->tokenType == TOK_IDENTIFIER &&
             strncmp(functionToken->text, param->argument->text, strlen(functionToken->text)))
         {
            mcc_Token_t *result;
            mcc_Token_t *copy = mcc_CopyToken(param->parameter);
            result = mcc_TokenListStandaloneReplaceCurrent(tokensIter, copy);
            mcc_DeleteToken((uintptr_t) result);
         }
         functionToken = mcc_GetNextToken(tokensIter);
      }
      mcc_TokenListDeleteIterator(tokensIter);
      param = (mcc_MacroParameter_t *) mcc_ListGetNextData(parametersIter);
   }
   mcc_TokenListDeleteIterator(parametersIter);
   return functionTokens;
}

static mcc_TokenList_t *handleMacroFunction(mcc_Macro_t *macro)
{
   getToken();
   maybeGetWhitespaceToken();
   mcc_ExpectTokenType(currentToken, TOK_SYMBOL, SYM_OPEN_PAREN);
   mcc_List_t *parameters = mcc_ListCreate();
   const mcc_Token_t *temp = mcc_TokenListPeekNextToken(tokenListIter);
   if (temp->tokenType == TOK_WHITESPACE)
      getToken();
   if (temp && temp->tokenType == TOK_SYMBOL && temp->tokenIndex == SYM_CLOSE_PAREN)
   {
      getToken();
   }
   else
   {
      mcc_TokenListIterator_t *argumentsIter = mcc_TokenListStandaloneGetIterator(macro->arguments);
      while (!(currentToken->tokenType == TOK_SYMBOL &&
               currentToken->tokenIndex == SYM_CLOSE_PAREN))
      {
         mcc_MacroParameter_t *param = mcc_MacroParameterCreate();
         mcc_Token_t *parameter_token = mcc_ICE_EvaluateTokenString(tokenListIter);
         param->argument = mcc_GetNextToken(argumentsIter);
         param->parameter = parameter_token;
         currentToken = mcc_TokenListPeekCurrentToken(tokenListIter);
         mcc_ListAppendData(parameters, (uintptr_t)param);
         maybeGetWhitespaceToken();
      }
      mcc_TokenListDeleteIterator(argumentsIter);
   }
   if (mcc_ListGetLength(parameters) != mcc_ListGetLength(macro->arguments))
   {
      mcc_PrettyError(
         mcc_ResolveFileNameFromNumber(currentToken->fileno),
         currentToken->lineno,
         currentToken->line_index,
         "macro function %s expects %d argument(s), but %d were provided\n",
         macro->text,
         mcc_ListGetLength(macro->arguments),
         mcc_ListGetLength(parameters));
   }
   mcc_TokenList_t *result = replaceMacroTokens(macro, parameters);
   mcc_ListDelete(parameters, mcc_MacroParameterDelete);
   return result;
}

static void handleStringify(void)
{
   MCC_ASSERT(FALSE);
}