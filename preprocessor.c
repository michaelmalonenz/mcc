#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "macro.h"
#include "tokens.h"

#ifdef MCC_DEBUG
#define HANDLER_LINKAGE extern
#else
#define HANDLER_LINKAGE static
#endif

typedef void (preprocessorDirectiveHandler_t)(mcc_Token_t *currentToken);

HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleIfdef(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleIfndef(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleIf(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleEndif(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleElse(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleElif(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleUndef(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleError(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handlePragma(mcc_Token_t *currentToken);
HANDLER_LINKAGE void handleJoin(mcc_Token_t *currentToken);

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

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleDefine, &handleIfdef,
                                                                                   &handleIfndef, &handleIf, &handleEndif,
                                                                                   &handleElse, &handleElif, &handleUndef,
                                                                                   &handleError, &handlePragma, &handleJoin };

//static FILE *outputFile;

void mcc_PreprocessCurrentTokens(void)
{
   mcc_Token_t *currentToken = NULL;
   currentToken = mcc_GetNextToken();
   while(currentToken != NULL)
   {
      if (currentToken->tokenType == TOK_PP_DIRECTIVE)
      {
         ppHandlers[currentToken->tokenIndex](currentToken);
      }
      currentToken = mcc_GetNextToken();
   }
}

HANDLER_LINKAGE void handleInclude(mcc_Token_t *currentToken)
{
   mcc_TokeniseFile(currentToken->text);
   {
//	mcc_PreprocessFile(mcc_FindSystemInclude((const char *)mcc_StringBufferGetString(fileInclude)),
//      outputFile);
   }
}

//currently doesn't handle function-like macros
HANDLER_LINKAGE void handleDefine(mcc_Token_t *currentToken) 
{
   const char UNUSED(*macro_identifier);
   currentToken = mcc_GetNextToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   currentToken = mcc_GetNextToken();
   mcc_ExpectTokenType(currentToken, TOK_IDENTIFIER);
   macro_identifier = currentToken->text;
   currentToken = mcc_GetNextToken();
   mcc_ExpectTokenType(currentToken, TOK_WHITESPACE);
   //everything to the end of the line (i.e. need end of line tokens)
}

HANDLER_LINKAGE void handleUndef(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleError(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleIfdef(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleIfndef(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleIf(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleEndif(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleElse(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleElif(mcc_Token_t UNUSED(*currentToken)) {}

HANDLER_LINKAGE void handleJoin(mcc_Token_t UNUSED(*currentToken)) {}

//What shall I do with #pragmas???
HANDLER_LINKAGE void handlePragma(mcc_Token_t UNUSED(*currentToken)) {}

