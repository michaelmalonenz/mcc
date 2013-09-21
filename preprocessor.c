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

#ifdef MCC_DEBUG
#define HANDLER_LINKAGE extern
#else
#define HANDLER_LINKAGE static
#endif

typedef void (preprocessorDirectiveHandler_t)(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);

HANDLER_LINKAGE void handleInclude(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleIfdef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleIfndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleIf(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleEndif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleElse(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleElif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleUndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleError(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handlePragma(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);
HANDLER_LINKAGE void handleJoin(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter, bool_t ignore);

static inline void mcc_ExpectTokenType(mcc_Token_t *token, TOKEN_TYPE tokenType)
{
   if (token->tokenType != tokenType)
   {
      /* @TODO make this better than "Expect kEnd, got End" */
      mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),
                      token->lineno,
                      "Preprocessor expected a %s token, but got a %s token\n",
                      token_types[tokenType],
                      token_types[token->tokenType]);
   }
}

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleInclude, &handleDefine, &handleIfdef,
                                                                                   &handleIfndef, &handleIf, &handleEndif,
                                                                                   &handleElse, &handleElif, &handleUndef,
                                                                                   &handleError, &handlePragma, &handleJoin };

//static FILE *outputFile;

void mcc_PreprocessCurrentTokens(void)
{
   mcc_TokenListIterator_t *tokenListIter = mcc_TokenListGetIterator();
   mcc_Token_t *currentToken = mcc_GetNextToken(tokenListIter);
   
   while(currentToken != NULL)
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         currentToken = mcc_RemoveCurrentToken(tokenListIter);
         ppHandlers[currentToken->tokenIndex](currentToken, tokenListIter, FALSE);
      }
      currentToken = mcc_GetNextToken(tokenListIter);
   }
   mcc_TokenListDeleteIterator(tokenListIter);
}

HANDLER_LINKAGE void handleInclude(mcc_Token_t *currentToken,
                                   mcc_TokenListIterator_t *tokenListIter,
                                   bool_t ignore)
{
   mcc_TokenListIterator_t *incIter;
   char *include_path;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
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
HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken,
                                  mcc_TokenListIterator_t *tokenListIter,
                                  bool_t UNUSED(ignore))
{
   const char *macro_identifier;
   mcc_TokenList_t *tokens = mcc_TokenListCreateStandalone();
   mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(tokens);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER);
   macro_identifier = currentToken->text;
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

HANDLER_LINKAGE void handleUndef(mcc_Token_t *currentToken,
                                 mcc_TokenListIterator_t *tokenListIter,
                                 bool_t UNUSED(ignore))
{
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER);
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

HANDLER_LINKAGE void handleError(mcc_Token_t *currentToken,
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

static void mcc_Defined(mcc_Token_t *currentToken,
                        mcc_TokenListIterator_t *tokenListIter,
                        bool_t positive)
{
   bool_t macroDefined;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER);
   macroDefined = mcc_IsMacroDefined(currentToken->text);

   do
   {
      currentToken = mcc_GetNextToken(tokenListIter);
      if (currentToken->tokenType == TOK_PP_DIRECTIVE &&
          currentToken->tokenIndex != PP_ENDIF)
      {
         currentToken = mcc_RemoveCurrentToken(tokenListIter);         
         ppHandlers[currentToken->tokenIndex](currentToken, tokenListIter, (macroDefined && positive));
      }
   } while (currentToken->tokenType != TOK_PP_DIRECTIVE && 
            currentToken->tokenIndex != PP_ENDIF);
}

HANDLER_LINKAGE void handleIfdef(mcc_Token_t *currentToken,
                                 mcc_TokenListIterator_t *tokenListIter,
                                 bool_t UNUSED(ignore))
{
   mcc_Defined(currentToken, tokenListIter, TRUE);  
}

HANDLER_LINKAGE void handleIfndef(mcc_Token_t *currentToken,
                                  mcc_TokenListIterator_t *tokenListIter,
                                  bool_t UNUSED(ignore))
{
   mcc_Defined(currentToken, tokenListIter, FALSE);
}

HANDLER_LINKAGE void handleIf(mcc_Token_t UNUSED(*currentToken),
                              mcc_TokenListIterator_t *tokenListIter,
                              bool_t UNUSED(ignore)) 
{
   //Get values for the identifier tokens.
   int result = mcc_ICE_EvaluateTokenString(tokenListIter);
   if (result)
   {
   }
}

HANDLER_LINKAGE void handleEndif(mcc_Token_t *currentToken,
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

HANDLER_LINKAGE void handleElse(mcc_Token_t *currentToken,
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

HANDLER_LINKAGE void handleElif(mcc_Token_t *currentToken,
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

HANDLER_LINKAGE void handleJoin(mcc_Token_t UNUSED(*currentToken),
                                mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                bool_t UNUSED(ignore)) {}

//What shall I do with #pragmas???
HANDLER_LINKAGE void handlePragma(mcc_Token_t UNUSED(*currentToken),
                                  mcc_TokenListIterator_t UNUSED(*tokenListIter),
                                  bool_t UNUSED(ignore)) {}

