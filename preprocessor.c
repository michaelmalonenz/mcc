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

static void mcc_ExpectTokenType(mcc_Token_t *token, TOKEN_TYPE tokenType, int index)
{
   if (token->tokenType != tokenType)
   {
      const char *expected = token_types[tokenType];
      if (tokenType == TOK_PP_DIRECTIVE)
      {
         expected = preprocessor_directives[index];
      }
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),
                      token->lineno,
                      "Preprocessor expected a %s token, but got a %s token\n",
                      expected,
                      token_types[token->tokenType]);
   }
}

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = {
   &handleInclude, &handleDefine, &handleIfdef,
   &handleIfndef, &handleIf, &handleEndif,
   &handleElse, &handleElif, &handleUndef,
   &handleError, &handlePragma, &handleJoin,
   &handleWarning,
};

static mcc_TokenListIterator_t *tokenListIter;
static mcc_Token_t *currentToken;
static mcc_List_t *output;

static void emitToken(void)
{
   mcc_TokenListStandaloneAppend(output, mcc_CopyToken(currentToken));
}

static void getToken(void)
{
   currentToken = mcc_GetNextToken(tokenListIter);
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
                        "Expected a filename to include, got '%s'\n",
                        currentToken->text);
   }
   if (include_path == NULL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                        currentToken->lineno,
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

//currently doesn't handle function-like macros
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
   if (currentToken->tokenType == TOK_WHITESPACE)
   {
      getToken();
   }
   else if (currentToken->tokenType == TOK_SYMBOL &&
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
   if (currentToken->tokenType == TOK_WHITESPACE)
   {
      getToken();
   }
   if (currentToken->tokenType != TOK_EOL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      "Unexpected characters after #undef '%s'\n",
                      currentToken->text);
   }
}

static void handleError()
{
   mcc_Token_t *temp;
   getToken();
   temp = currentToken;
   while (currentToken->tokenType != TOK_EOL)
   {
      temp = mcc_ConCatTokens(temp, currentToken, TOK_STR_CONST);
      getToken();
   }
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(temp->fileno),
                   temp->lineno,
                   "Error: %s\n", temp->text);
}

static void conditionalInnerImpl(bool_t initialConditionTrue)
{
   bool_t processMacro = initialConditionTrue;
   bool_t handled = FALSE;

   getToken();
   while (currentToken->tokenType != TOK_PP_DIRECTIVE ||
          (currentToken->tokenType == TOK_PP_DIRECTIVE &&
           currentToken->tokenIndex != PP_ENDIF))
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
          currentToken->tokenIndex == PP_ELSE)
      {
         handled = handled || processMacro;
         processMacro = !processMacro;
      }
      else if (processMacro && !handled)
      {
         if (currentToken->tokenType == TOK_PP_DIRECTIVE)
         {
            handlePreprocessorDirective(currentToken, tokenListIter);
         }
         else
         {
            emitToken();
         }
      }
      getToken();
   }
   mcc_ExpectTokenType(currentToken, TOK_PP_DIRECTIVE, PP_ENDIF);
}

static void handleIfdef()
{
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(mcc_IsMacroDefined(currentToken->text));
}

static void handleIfndef()
{
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   getToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   conditionalInnerImpl(!mcc_IsMacroDefined(currentToken->text));
}

static void handleIf()
{
   //Get values for the identifier tokens.
   int result = mcc_ICE_EvaluateTokenString(tokenListIter);
   conditionalInnerImpl(result);
}

static void handleEndif()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     "endif without if\n");
}

static void handleElse()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     "else without if\n");
}

static void handleElif()
{
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                     currentToken->lineno,
                     "elif without if\n");
}

static void handleJoin() {}

//What shall I do with #pragmas???
static void handlePragma() {}

static void handleWarning()
{
   mcc_Token_t *temp;
   getToken();
   temp = currentToken;
   while (currentToken->tokenType != TOK_EOL)
   {
      temp = mcc_ConCatTokens(temp, currentToken, TOK_STR_CONST);
      getToken();
   }
   printf(mcc_ResolveFileNameFromNumber(temp->fileno),
          temp->lineno,
          "Warning: %s\n", temp->text);
}
