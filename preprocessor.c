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

typedef void (preprocessorDirectiveHandler_t)(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);

static void handleInclude(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleDefine(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleIfdef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleIfndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleIf(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleEndif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleElse(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleElif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleUndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleError(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handlePragma(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
static void handleJoin(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);

static inline void mcc_ExpectTokenType(mcc_Token_t *token, TOKEN_TYPE tokenType, int index)
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
   &handleError, &handlePragma, &handleJoin
};

static void handlePreprocessorDirective(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter)
{
   MCC_ASSERT(currentToken->tokenType == TOK_PP_DIRECTIVE);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   ppHandlers[currentToken->tokenIndex](currentToken, tokenListIter, FALSE);
}

void mcc_PreprocessCurrentTokens(void)
{
   mcc_TokenListIterator_t *tokenListIter = mcc_TokenListGetIterator();
   mcc_Token_t *currentToken = mcc_GetNextToken(tokenListIter);

   while(currentToken != NULL)
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         handlePreprocessorDirective(currentToken, tokenListIter);
      }
      currentToken = mcc_GetNextToken(tokenListIter);
   }
   mcc_TokenListDeleteIterator(tokenListIter);
}

static void handleInclude(mcc_Token_t *currentToken,
                          mcc_TokenListIterator_t *tokenListIter,
                          bool_t ignore)
{
   mcc_TokenListIterator_t *incIter;
   char *include_path;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   if (!ignore)
   {
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

      incIter = mcc_TokenListCopyIterator(tokenListIter);
      mcc_TokeniseFile(include_path, incIter);
      free(include_path);
      mcc_TokenListDeleteIterator(incIter);
   }
}

//currently doesn't handle function-like macros
static void handleDefine(mcc_Token_t *currentToken,
                         mcc_TokenListIterator_t *tokenListIter,
                         bool_t UNUSED(ignore))
{
   const char *macro_identifier;
   mcc_TokenList_t *tokens = mcc_TokenListCreateStandalone();
   mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(tokens);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   macro_identifier = currentToken->text;
   printf("Defining Macro: %s\n", macro_identifier);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   if (currentToken->tokenType == TOK_WHITESPACE)
   {
      currentToken = mcc_RemoveCurrentToken(tokenListIter);
   }
   while (currentToken->tokenType != TOK_EOL)
   {
      mcc_InsertToken(currentToken, iter);
      currentToken = mcc_RemoveCurrentToken(tokenListIter);
   }
   mcc_TokenListDeleteIterator(iter);
   mcc_DefineMacro(macro_identifier, tokens);
}

static void handleUndef(mcc_Token_t *currentToken,
                        mcc_TokenListIterator_t *tokenListIter,
                        bool_t UNUSED(ignore))
{
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   mcc_UndefineMacro(currentToken->text);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   if (currentToken->tokenType == TOK_WHITESPACE)
   {
      currentToken = mcc_RemoveCurrentToken(tokenListIter);
   }
   if (currentToken->tokenType != TOK_EOL)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      "Unexpected characters after #undef '%s'\n",
                      currentToken->text);
   }
}

static void handleError(mcc_Token_t *currentToken,
                        mcc_TokenListIterator_t *tokenListIter,
                        bool_t UNUSED(ignore))
{
   mcc_Token_t *temp = NULL;
   currentToken = mcc_GetNextToken(tokenListIter);
   while (currentToken->tokenType != TOK_EOL)
   {
      if (temp == NULL)
      {
         temp = currentToken;
      }
      else
      {
         temp = mcc_ConCatTokens(temp, currentToken, TOK_STR_CONST);
      }
      currentToken = mcc_GetNextToken(tokenListIter); 
   }
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(temp->fileno),
                   temp->lineno,
                   "Error: %s\n", temp->text);
}

static void handleIfdef(mcc_Token_t *currentToken,
                        mcc_TokenListIterator_t *tokenListIter,
                        bool_t UNUSED(ignore))
{
   bool_t processMacro;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   processMacro = mcc_IsMacroDefined(currentToken->text);
   printf("Process Macro: %d\n", processMacro);

   while (currentToken->tokenType != TOK_PP_DIRECTIVE ||
          (currentToken->tokenType == TOK_PP_DIRECTIVE &&
           currentToken->tokenIndex != PP_ENDIF))
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         if (currentToken->tokenIndex == PP_ELSE)
         {
            processMacro = !processMacro;
            printf("Unprocess Macro: %u\n", processMacro);
            (void)mcc_RemoveCurrentToken(tokenListIter);
         }
         else if (processMacro)
         {
            mcc_DebugPrintTokenList(tokenListIter);
            handlePreprocessorDirective(currentToken, tokenListIter);
            mcc_DebugPrintTokenList(tokenListIter);
         }
         currentToken = mcc_GetNextToken(tokenListIter);
      } else {
         currentToken = mcc_GetNextToken(tokenListIter);
      }
      mcc_DebugPrintToken(currentToken);
   }
   mcc_ExpectTokenType(currentToken, TOK_PP_DIRECTIVE, PP_ENDIF);
}

static void handleIfndef(mcc_Token_t *currentToken,
                         mcc_TokenListIterator_t *tokenListIter,
                         bool_t UNUSED(ignore))
{
   bool_t macroDefined;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE, TOK_UNSET_INDEX);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX);
   macroDefined = mcc_IsMacroDefined(currentToken->text);

   while (currentToken->tokenType != TOK_PP_DIRECTIVE ||
          (currentToken->tokenType == TOK_PP_DIRECTIVE &&
           currentToken->tokenIndex != PP_ENDIF))
   {
      currentToken = mcc_GetNextToken(tokenListIter);
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         if (currentToken->tokenIndex == PP_ELSE)
         {
            macroDefined = !macroDefined;
         }
         else if (!macroDefined)
         {
            handlePreprocessorDirective(currentToken, tokenListIter);
         }
      }
   }
}

static void handleIf(mcc_Token_t UNUSED(*currentToken),
                     mcc_TokenListIterator_t *tokenListIter,
                     bool_t UNUSED(ignore))
{
   //Get values for the identifier tokens.
   int result = mcc_ICE_EvaluateTokenString(tokenListIter);
   if (result)
   {
   }
}

static void handleEndif(mcc_Token_t *currentToken,
                                 mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                 bool_t ignore)
{
   if (!ignore)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      "endif without if\n");
   }
}

static void handleElse(mcc_Token_t *currentToken,
                                mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                bool_t ignore)
{
   if (!ignore)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      "else without if\n");
   }
}

static void handleElif(mcc_Token_t *currentToken,
                                mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                bool_t ignore)
{
   if (!ignore)
   {
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno),
                      currentToken->lineno,
                      "elif without if\n");
   }
}

static void handleJoin(mcc_Token_t UNUSED(*currentToken),
                                mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                bool_t UNUSED(ignore)) {}

//What shall I do with #pragmas???
static void handlePragma(mcc_Token_t UNUSED(*currentToken),
                                  mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                  bool_t UNUSED(ignore)) {}

