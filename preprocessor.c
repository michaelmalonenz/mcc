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

#ifdef MCC_DEBUG
#define HANDLER_LINKAGE extern
#else
#define HANDLER_LINKAGE static
#endif

typedef void (preprocessorDirectiveHandler_t)(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);

HANDLER_LINKAGE void handleInclude(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleIfdef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleIfndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleIf(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleEndif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleElse(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleElif(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleUndef(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleError(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handlePragma(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);
HANDLER_LINKAGE void handleJoin(mcc_Token_t *currentToken, mcc_TokenListIterator_t *tokenListIter);

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
         ppHandlers[currentToken->tokenIndex](currentToken, tokenListIter);
      }
      currentToken = mcc_GetNextToken(tokenListIter);
   }
   mcc_TokenListDeleteIterator(tokenListIter);
}

HANDLER_LINKAGE void handleInclude(mcc_Token_t *currentToken,
                                   mcc_TokenListIterator_t *tokenListIter)
{
   mcc_TokenListIterator_t *incIter;
   char *include_path;
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   currentToken = mcc_RemoveCurrentToken(tokenListIter);
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
   incIter = mcc_TokenListCopyIterator(tokenListIter);
   mcc_TokeniseFile(include_path, incIter);
   free(include_path);
   mcc_TokenListDeleteIterator(incIter);
}

//currently doesn't handle function-like macros
HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken,
                                  mcc_TokenListIterator_t *tokenListIter)
{
   const char UNUSED(*macro_identifier);
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
      currentToken = mcc_RemoveCurrentToken(tokenListIter);
      //need to save this string of tokens somehow and add them to the macro table
   }
}

HANDLER_LINKAGE void handleUndef(mcc_Token_t *currentToken,
                                 mcc_TokenListIterator_t *tokenListIter)
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
                                 mcc_TokenListIterator_t *tokenListIter)
{
   mcc_Token_t *temp = NULL;
   currentToken = mcc_GetNextToken(tokenListIter);
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
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

HANDLER_LINKAGE void handleIfdef(mcc_Token_t UNUSED(*currentToken),
                                 mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleIfndef(mcc_Token_t UNUSED(*currentToken),
                                  mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleIf(mcc_Token_t UNUSED(*currentToken),
                              mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleEndif(mcc_Token_t UNUSED(*currentToken),
                                 mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleElse(mcc_Token_t UNUSED(*currentToken),
                                mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleElif(mcc_Token_t UNUSED(*currentToken),
                                mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

HANDLER_LINKAGE void handleJoin(mcc_Token_t UNUSED(*currentToken),
                                mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

//What shall I do with #pragmas???
HANDLER_LINKAGE void handlePragma(mcc_Token_t UNUSED(*currentToken),
                                  mcc_TokenListIterator_t UNUSED(*tokenListIter)) {}

