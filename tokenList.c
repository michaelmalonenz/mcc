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

static const char whitespaceText = ' ';

static bool_t finished_iterating = FALSE;

mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const int lineno,
                             const unsigned short fileno)
{
   mcc_Token_t *token = (mcc_Token_t *) malloc(sizeof(mcc_Token_t));
   token->text = (char *) malloc(sizeof(char) * (text_len + 1));
   memcpy(token->text, text, text_len);
   token->text[text_len] = '\0';
   token->tokenType = type;
   token->tokenIndex = 0;
   token->lineno = lineno;
   token->fileno = fileno;
   token->next = NULL;
   token->previous = NULL;

   return token;
}

void mcc_AddEndOfLineToken(const int lineno, const unsigned short fileno)
{
   mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                        TOK_EOL, lineno, fileno);
   mcc_AddToken(token);   
}

void mcc_CreateAndAddWhitespaceToken(const int lineno, const unsigned short fileno)
{
   mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                        TOK_WHITESPACE, lineno, fileno);
   mcc_AddToken(token);
}

void mcc_DeleteToken(mcc_Token_t *token)
{
   free(token->text);
   free(token);
}

//Need to handle the "insert" case
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
//   printf("Got me a token '%s' of type %d\n", token->text, token->tokenType);
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
   finished_iterating = FALSE;
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
   int newLength = strlen(first->text) + strlen(second->text);
   //it really only makes sense to be concatenating tokens which are on the same line.
   MCC_ASSERT(first->lineno == second->lineno);
   first->text = (char *) realloc(first->text, newLength);
   strncat(first->text, second->text, newLength);
   first->tokenType = newType;
   first->next = second->next;
   mcc_DeleteToken(second);
#if MCC_DEBUG
   numberOfTokens--;
   printf("Concatenated two tokens to make '%s' of type %d\n",
          first->text, first->tokenType);
#endif
   return first;
}
