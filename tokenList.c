#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "config.h"
#include "mcc.h"

static mcc_Token_t *firstToken = NULL;

//I'm not convinced currentToken is the best name for this
static mcc_Token_t *currentToken = NULL;

static mcc_Token_t *nextToken = NULL;

#if MCC_DEBUG
static int numberOfTokens = 0;
#endif

mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const int lineno)
{
   mcc_Token_t *token = (mcc_Token_t *) malloc(sizeof(mcc_Token_t));
   token->name = (char *) malloc(sizeof(char) * (text_len + 1));
   memcpy(token->name, text, text_len + 1);
   token->name[text_len + 1] = '\0';
   token->tokenType = type;
   token->lineno = lineno;
   token->next = NULL;

   return token;
}

void mcc_DeleteToken(mcc_Token_t *token)
{
   free(token->name);
   free(token);
}

void mcc_AddToken(mcc_Token_t *token)
{
   if (firstToken == NULL)
   {
      firstToken = token;
   }
   token->next = currentToken;
   currentToken = token;
#if MCC_DEBUG
   numberOfTokens++;
#endif
}

void mcc_FreeTokens()
{
   mcc_Token_t *temp = NULL;
   if(currentToken == NULL)
      return;
   temp = currentToken->next;
   while(currentToken != NULL)
   {
      mcc_DeleteToken(currentToken);
      currentToken = temp;
#if MCC_DEBUG
      numberOfTokens--;
   }
   MCC_ASSERT(numberOfTokens == 0);
#else
   }
#endif
}

mcc_Token_t *mcc_GetNextToken()
{
   mcc_Token_t *result;
   if (nextToken == NULL)
   {
      nextToken = firstToken;
   }
   if (nextToken == NULL)
   {
      result = NULL;
   }
   else
   {
      result = nextToken;
      nextToken = nextToken->next;
   }
   return result;
}


