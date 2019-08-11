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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mcc.h"
#include "liberal.h"
#include "preprocessor_private.h"
#include "toolChainCommands.h"
#include "preprocessor.h"
#include "macro.h"
#include "tokens.h"
#include "tokenList.h"

static mcc_TokenList_t *handleMacroFunction(preprocessor_t *preprocessor, mcc_Macro_t *macro);
static mcc_TokenList_t *handleMacroReplacement(mcc_Macro_t *macro);

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = {
   &handleInclude, &handleInclude, &handleDefine, &handleIfdef,
   &handleIfndef, &handleIf, &handleEndif,
   &handleElse, &handleElif, &handleUndef,
   &handleError, &handlePragma, &handleJoin,
   &handleWarning, &handleStringify
};

mcc_TokenList_t *resolveMacroTokens(preprocessor_t *preprocessor, const char *macroText)
{
   mcc_Macro_t *macro = mcc_ResolveMacro(macroText);
   if (macro == NULL)
   {
      return NULL;
   }

   if (macro->is_function)
   {
      return handleMacroFunction(preprocessor, macro);
   }
   else
   {
      return handleMacroReplacement(macro);
   }
}

void emitToken(preprocessor_t *preprocessor)
{
   if (preprocessor->currentToken->tokenType == TOK_IDENTIFIER)
   {
      mcc_TokenList_t *macroTokens = resolveMacroTokens(
            preprocessor, preprocessor->currentToken->text);
      if (macroTokens == NULL)
      {
         mcc_TokenListAppend(preprocessor->output, mcc_CopyToken(preprocessor->currentToken));
      }
      else
      {
         mcc_TokenListIterator_t *macroTokensIter = mcc_TokenListGetIterator(macroTokens);
         mcc_Token_t *token = mcc_GetNextToken(macroTokensIter);
         while (token != NULL)
         {
            mcc_TokenListAppend(preprocessor->output, mcc_CopyToken(token));
            token = mcc_GetNextToken(macroTokensIter);
         }
         mcc_TokenListDeleteIterator(macroTokensIter);
         mcc_TokenListDelete(macroTokens);
      }
   }
   else
   {
      mcc_TokenListAppend(preprocessor->output, mcc_CopyToken(preprocessor->currentToken));
   }
}

void getToken(preprocessor_t *preprocessor)
{
   preprocessor->currentToken = mcc_GetNextToken(preprocessor->tokenListIter);
}

void maybeGetWhiteSpaceToken(preprocessor_t *preprocessor)
{
   if (preprocessor->currentToken->tokenType == TOK_WHITESPACE)
   {
      getToken(preprocessor);
   }
}

void handlePreprocessorDirective(preprocessor_t *preprocessor)
{
   MCC_ASSERT(preprocessor->currentToken->tokenType == TOK_PP_DIRECTIVE);
   ppHandlers[preprocessor->currentToken->tokenIndex](preprocessor);
}

eral_List_t *mcc_PreprocessTokens(mcc_TokenList_t *tokens)
{
   preprocessor_t preprocessor;
   preprocessor.output = mcc_TokenListCreate();
   preprocessor.tokenListIter = mcc_TokenListGetIterator(tokens);
   getToken(&preprocessor);

   while(preprocessor.currentToken != NULL)
   {
      if (preprocessor.currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         handlePreprocessorDirective(&preprocessor);
      }
      else
      {
         emitToken(&preprocessor);
      }
      getToken(&preprocessor);
   }
   mcc_TokenListDeleteIterator(preprocessor.tokenListIter);
   return preprocessor.output;
}

void handleInclude(preprocessor_t *preprocessor)
{
   char *include_path;
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken(preprocessor);
#if MCC_DEBUG
   printf("Including file from %s:%d: %s\n",
      mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
      preprocessor->currentToken->lineno,
      preprocessor->currentToken->text);
#endif
   if (preprocessor->currentToken->tokenType == TOK_LOCAL_FILE_INC)
   {
      include_path = mcc_FindLocalInclude(preprocessor->currentToken->text);
   }
   else if (preprocessor->currentToken->tokenType == TOK_SYS_FILE_INC)
   {
      include_path = mcc_FindSystemInclude(preprocessor->currentToken->text);
   }
   else
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                        preprocessor->currentToken->lineno,
                        preprocessor->currentToken->line_index,
                        "Expected a filename to include, got '%s'\n",
                        preprocessor->currentToken->text);
   }
   if (include_path == NULL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                        preprocessor->currentToken->lineno,
                        preprocessor->currentToken->line_index,
                        "Couldn't locate file '%s' for inclusion\n",
                        preprocessor->currentToken->text);
   }

   while (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      getToken(preprocessor);
   }
   mcc_TokenList_t *tokens = mcc_TokeniseFile(include_path);
   free(include_path);
   //I could pop the path onto a stack here, or something.
   mcc_TokenList_t *out_tokens = mcc_PreprocessTokens(tokens);
   mcc_TokenListConcatenate(preprocessor->output, out_tokens);
   mcc_TokenListDelete(tokens);
}

void handleDefine(preprocessor_t *preprocessor)
{
   const char *macro_identifier;
   mcc_TokenList_t *tokens = mcc_TokenListCreate();
   mcc_TokenList_t *arguments = NULL;
   bool_t variadic = FALSE;
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   macro_identifier = preprocessor->currentToken->text;
   getToken(preprocessor);
   if (preprocessor->currentToken->tokenType == TOK_SYMBOL &&
       preprocessor->currentToken->tokenIndex == SYM_OPEN_PAREN)
   {
      arguments = mcc_TokenListCreate();
      getToken(preprocessor);
      while (preprocessor->currentToken->tokenType != TOK_SYMBOL &&
             preprocessor->currentToken->tokenIndex != SYM_CLOSE_PAREN)
      {
         if (preprocessor->currentToken->tokenType == TOK_EOL)
         {
            mcc_PrettyError(
               mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
               preprocessor->currentToken->lineno,
               preprocessor->currentToken->line_index,
               "Unclosed parentheses in function macro '%s'\n",
               macro_identifier);
         }
         else if ((preprocessor->currentToken->tokenType == TOK_OPERATOR &&
              preprocessor->currentToken->tokenIndex == OP_COMMA) ||
             preprocessor->currentToken->tokenType == TOK_WHITESPACE)
         {
            // Do nothing.
            // This is fine and normal and expected, but I don't care.
            // I should attempt to deal with two commas in a row, but
            // other than that, it's fine
         }
         else if (preprocessor->currentToken->tokenType == TOK_IDENTIFIER)
         {
            mcc_TokenListAppend(arguments, mcc_CopyToken(preprocessor->currentToken));
         }
         else if (preprocessor->currentToken->tokenType == TOK_OPERATOR &&
                  preprocessor->currentToken->tokenIndex == OP_VARIADIC_ARGS)
         {
            variadic = TRUE;
            maybeGetWhiteSpaceToken(preprocessor);
         }
         else
         {
            mcc_DebugPrintToken(preprocessor->currentToken);
            MCC_ASSERT(FALSE);
         }
         getToken(preprocessor);
         if (variadic)
         {
            if (!(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                  preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
            {
               mcc_PrettyError(
                  mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                  preprocessor->currentToken->lineno,
                  preprocessor->currentToken->line_index,
                  "the variadic operator must be the last parameter in a function\n"
               );
            }
         }
      }
   }
   if (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      getToken(preprocessor);
      maybeGetWhiteSpaceToken(preprocessor);
   }
   while (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      mcc_TokenListAppend(tokens, mcc_CopyToken(preprocessor->currentToken));
      getToken(preprocessor);
   }
   mcc_DefineMacro(macro_identifier, tokens, arguments, variadic);
}

void handleUndef(preprocessor_t *preprocessor)
{
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   mcc_UndefineMacro(preprocessor->currentToken->text);
   getToken(preprocessor);
   maybeGetWhiteSpaceToken(preprocessor);
   if (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                      preprocessor->currentToken->lineno,
                      preprocessor->currentToken->line_index,
                      "Unexpected characters after #undef '%s'\n",
                      preprocessor->currentToken->text);
   }
}

void handleError(preprocessor_t *preprocessor)
{
   const mcc_Token_t *first = preprocessor->currentToken;
   eral_StringBuffer_t *buffer = eral_CreateStringBuffer();
   getToken(preprocessor);
   while (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      eral_StringBufferAppendString(buffer, preprocessor->currentToken->text);
      getToken(preprocessor);
   }
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(first->fileno),
                   first->lineno,
                   first->line_index,
                   "Error: %s\n", eral_StringBufferGetString(buffer));
}

void handleJoin(preprocessor_t *preprocessor)
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                     preprocessor->currentToken->lineno,
                     preprocessor->currentToken->line_index,
                     "## found outside a macro function\n");
}

void handleStringify(preprocessor_t *preprocessor)
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                     preprocessor->currentToken->lineno,
                     preprocessor->currentToken->line_index,
                     "# found outside a macro function\n");
}

//What shall I do with #pragmas???
void handlePragma(preprocessor_t UNUSED(*preprocessor))
{
   MCC_ASSERT(FALSE);
}

void handleWarning(preprocessor_t *preprocessor)
{
   const mcc_Token_t *first = preprocessor->currentToken;
   eral_StringBuffer_t *buffer = eral_CreateStringBuffer();
   getToken(preprocessor);
   while (preprocessor->currentToken->tokenType != TOK_EOL)
   {
      eral_StringBufferAppendString(buffer, preprocessor->currentToken->text);
      getToken(preprocessor);
   }
   printf("%s:%d Warning: %s\n",
      mcc_ResolveFileNameFromNumber(first->fileno),
      first->lineno,
      eral_StringBufferGetString(buffer));
   eral_DeleteStringBuffer(buffer);
}

static mcc_TokenList_t *handleMacroReplacement(mcc_Macro_t *macro)
{
   mcc_TokenList_t *result = mcc_TokenListCreate();
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(macro->tokens);
   mcc_Token_t *macroToken = mcc_GetNextToken(iter);
   while (macroToken != NULL)
   {
      mcc_TokenListAppend(result, mcc_CopyToken(macroToken));
      macroToken = mcc_GetNextToken(iter);
   }
   mcc_TokenListDeleteIterator(iter);
   return result;
}

mcc_TokenList_t *replaceMacroTokens(mcc_Macro_t *macro, eral_List_t *parameters)
{
   mcc_TokenList_t *functionTokens = mcc_TokenListDeepCopy(macro->tokens);
   eral_ListIterator_t *parametersIter = eral_ListGetIterator(parameters);
   mcc_MacroParameter_t *param = (mcc_MacroParameter_t *) eral_ListGetNextData(parametersIter);
   while (param != NULL)
   {
      mcc_TokenListIterator_t *tokensIter = mcc_TokenListGetIterator(functionTokens);
      mcc_Token_t *functionToken = mcc_GetNextToken(tokensIter);
      while (functionToken != NULL)
      {
         if (param->argument == NULL &&
             strncmp(functionToken->text, "__VA_ARGS__", strlen("__VA_ARGS__")) == 0)
         {
            eral_ListIterator_t *iter_copy = eral_ListCopyIterator(parametersIter);
            mcc_MacroParameter_t *currentParam = (mcc_MacroParameter_t *)eral_ListPeekCurrentData(iter_copy);
            mcc_TokenList_t *paramTokens = mcc_TokenListCreate();
            while(currentParam != NULL)
            {
               mcc_TokenListConcatenate(paramTokens, mcc_TokenListDeepCopy(currentParam->parameterTokens));
               currentParam = (mcc_MacroParameter_t *) eral_ListGetNextData(iter_copy);
            }
            mcc_Token_t *result = mcc_TokenListReplaceCurrent(tokensIter, paramTokens);
            mcc_DeleteToken((uintptr_t) result);
            eral_ListDelete(paramTokens, NULL);
            eral_ListDeleteIterator(iter_copy);
         }
         else if (functionToken->tokenType == TOK_IDENTIFIER &&
            strcmp(functionToken->text,
                  param->argument->text) == 0)
         {
            mcc_TokenList_t *paramTokens = mcc_TokenListDeepCopy(param->parameterTokens);
            mcc_Token_t *result = mcc_TokenListReplaceCurrent(tokensIter, paramTokens);
            mcc_DeleteToken((uintptr_t) result);
            eral_ListDelete(paramTokens, NULL);
         }
         functionToken = mcc_GetNextToken(tokensIter);
      }
      mcc_TokenListDeleteIterator(tokensIter);
      param = (mcc_MacroParameter_t *) eral_ListGetNextData(parametersIter);
   }
   mcc_TokenListDeleteIterator(parametersIter);
   return functionTokens;
}

void rescanMacroFunctionForActions(mcc_TokenList_t *tokens)
{
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(tokens);
   mcc_Token_t *token = mcc_GetNextToken(iter);
   while (token != NULL)
   {
      if (token->tokenType == TOK_PP_DIRECTIVE &&
          token->tokenIndex == PP_JOIN)
      {
         mcc_DeleteToken((uintptr_t)mcc_TokenListRemoveCurrent(iter));
         mcc_Token_t *lhs = mcc_TokenListRemoveCurrent(iter);
         if (lhs->tokenType == TOK_WHITESPACE)
         {
            mcc_DeleteToken((uintptr_t)lhs);
            lhs = mcc_TokenListRemoveCurrent(iter);
         }

         (void)mcc_GetNextToken(iter);
         mcc_Token_t *rhs = mcc_TokenListRemoveCurrent(iter);
         if (rhs->tokenType == TOK_WHITESPACE)
         {
            mcc_DeleteToken((uintptr_t)rhs);
            (void)mcc_GetNextToken(iter);
            rhs = mcc_TokenListRemoveCurrent(iter);
         }
         eral_StringBuffer_t *sbuffer = eral_CreateStringBuffer();
         eral_StringBufferAppendString(sbuffer, lhs->text);
         eral_StringBufferAppendString(sbuffer, rhs->text);
         mcc_Token_t *tok = mcc_CreateToken(eral_StringBufferGetString(sbuffer),
            eral_GetStringBufferLength(sbuffer), TOK_IDENTIFIER,
            TOK_UNSET_INDEX, lhs->line_index + 1,
            lhs->lineno, lhs->fileno);
         mcc_InsertToken(tok, iter);
         eral_DeleteStringBuffer(sbuffer);
         mcc_DeleteToken((uintptr_t)lhs);
         mcc_DeleteToken((uintptr_t)rhs);
      }
      token = mcc_GetNextToken(iter);
   }
   mcc_TokenListDeleteIterator(iter);
}

static mcc_TokenList_t *handleMacroFunction(preprocessor_t *preprocessor, mcc_Macro_t *macro)
{
   getToken(preprocessor);
   maybeGetWhiteSpaceToken(preprocessor);
   mcc_ExpectTokenType(preprocessor->currentToken, TOK_SYMBOL, SYM_OPEN_PAREN);
   eral_List_t *parameters = eral_ListCreate();
   getToken(preprocessor);
   maybeGetWhiteSpaceToken(preprocessor);
   bool_t argumentsComplete = FALSE;
   if (preprocessor->currentToken &&
      !(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
        preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
   {
      mcc_TokenListIterator_t *argumentsIter = mcc_TokenListGetIterator(macro->arguments);
      while (!(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
               preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
      {
         mcc_MacroParameter_t *param = mcc_MacroParameterCreate();
         if (argumentsComplete)
            param->argument = NULL;
         else
         {
            param->argument = mcc_GetNextToken(argumentsIter);
            argumentsComplete = param->argument == NULL;
         }
         maybeGetWhiteSpaceToken(preprocessor);
         if (preprocessor->currentToken->tokenType == TOK_SYMBOL &&
             preprocessor->currentToken->tokenIndex == SYM_OPEN_PAREN)
         {
            getToken(preprocessor);
            while (!(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                     preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
            {
               mcc_TokenListAppend(param->parameterTokens, mcc_CopyToken(preprocessor->currentToken));
               getToken(preprocessor);
               MCC_ASSERT(preprocessor->currentToken != NULL);
            }
         }
         else
         {
            while (!(preprocessor->currentToken->tokenType == TOK_OPERATOR &&
                     preprocessor->currentToken->tokenIndex == OP_COMMA) &&
                   !(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                     preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
            {
               mcc_TokenListAppend(param->parameterTokens, mcc_CopyToken(preprocessor->currentToken));
               getToken(preprocessor);
               MCC_ASSERT(preprocessor->currentToken != NULL);
            }
         }
         if (preprocessor->currentToken->tokenType == TOK_OPERATOR &&
             preprocessor->currentToken->tokenIndex == OP_COMMA)
         {
            getToken(preprocessor);
         }
         eral_ListAppendData(parameters, (uintptr_t)param);
         maybeGetWhiteSpaceToken(preprocessor);
      }
      mcc_TokenListDeleteIterator(argumentsIter);
   }
   int params_len = eral_ListGetLength(parameters);
   int args_len = eral_ListGetLength(macro->arguments);
   if (macro->is_variadic)
   {
      args_len--;
      if (params_len < args_len)
      {
         mcc_PrettyError(
            mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
            preprocessor->currentToken->lineno,
            preprocessor->currentToken->line_index,
            "macro function '%s' expects at least %d argument(s), but %d were provided\n",
            macro->text,
            args_len,
            params_len);
      }
   }
   else if (params_len != args_len)
   {
      mcc_PrettyError(
         mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
         preprocessor->currentToken->lineno,
         preprocessor->currentToken->line_index,
         "macro function '%s' expects %d argument(s), but %d were provided\n",
         macro->text,
         args_len,
         params_len);
   }
   mcc_TokenList_t *result = replaceMacroTokens(macro, parameters);
   eral_ListDelete(parameters, mcc_MacroParameterDelete);
   rescanMacroFunctionForActions(result);
   return result;
}
