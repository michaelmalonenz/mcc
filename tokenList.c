#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tokens.h"
#include "config.h"
#include "mcc.h"

static mcc_Token_t *listHead = NULL;
static mcc_Token_t *listTail = NULL;
static mcc_Token_t *current = NULL;

#if MCC_DEBUG
static int numberOfTokens = 0;
#endif

static bool_t finished_iterating = FALSE;

mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const int lineno)
{
   mcc_Token_t *token = (mcc_Token_t *) malloc(sizeof(mcc_Token_t));
   token->name = (char *) malloc(sizeof(char) * (text_len + 1));
   memcpy(token->name, text, text_len);
   token->name[text_len] = '\0';
   token->tokenType = type;
   token->tokenIndex = 0;
   token->lineno = lineno;
   token->next = NULL;
   token->previous = NULL;

   return token;
}

void mcc_DeleteToken(mcc_Token_t *token)
{
   free(token->name);
   free(token);
}

void mcc_AddToken(mcc_Token_t *token)
{
   if (listHead == NULL)
   {
      listHead = token;
      listTail = token;
   }
   else
   {
      listTail->next = token;
      token->previous = listTail;
      listTail = token;
   }
#if MCC_DEBUG
   numberOfTokens++;
//   printf("Got me a token '%s' of type %d\n", token->name, token->tokenType);
#endif
}

void mcc_FreeTokens(void)
{
   mcc_Token_t *temp = NULL;
   current = listHead;
   temp = current->next;
   while(current != NULL)
   {
      mcc_DeleteToken(current);
      current = temp;
      if (current != NULL)
         temp = current->next;
      numberOfTokens--;
   }
   listHead = NULL;
   listTail = NULL;
#if MCC_DEBUG
   MCC_ASSERT(numberOfTokens == 0);
#endif
}


mcc_Token_t *mcc_GetNextToken(void)
{
   mcc_Token_t *result = NULL;
   if (!finished_iterating)
   {
      if (current == NULL)
      {
         MCC_ASSERT(listHead != NULL);
         result = listHead;
         current = listHead->next;
      }
      else
      {
         result = current;
         current = current->next;
      }
      finished_iterating = (result == listTail);
   }

   return result;
}

mcc_Token_t *mcc_ConCatTokens(mcc_Token_t *first, mcc_Token_t *second, TOKEN_TYPE newType)
{
   int newLength = strlen(first->name) + strlen(second->name);
   //it really only makes sense to be concatenating tokens which are on the same line.
   MCC_ASSERT(first->lineno == second->lineno);
   first->name = (char *) realloc(first->name, newLength);
   strncat(first->name, second->name, newLength);
   first->tokenType = newType;
   first->next = second->next;
   mcc_DeleteToken(second);
#if MCC_DEBUG
   numberOfTokens--;
   printf("Concatenated two tokens to make '%s' of type %d\n",
          first->name, first->tokenType);
#endif
   return first;
}
