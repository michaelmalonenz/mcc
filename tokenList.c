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

#include "config.h"
#include "options.h"
#include "tokens.h"
#include "tokenList.h"
#include "list.h"
#include "mcc.h"

static mcc_TokenList_t *token_list = NULL;

static const char whitespaceText = ' ';

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

   return token;
}

void mcc_AddEndOfLineToken(const int lineno, const unsigned short fileno,
                           mcc_TokenListIterator_t *iter)
{
   mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                        TOK_EOL, lineno, fileno);
   mcc_InsertToken(token, iter);   
}

void mcc_CreateAndAddWhitespaceToken(const int lineno,
                                     const unsigned short fileno,
                                     mcc_TokenListIterator_t *iter)
{
   const mcc_Token_t *temp = mcc_TokenListPeekCurrentToken(iter);
   if (temp != NULL && temp->tokenType != TOK_WHITESPACE)
   {
      mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                           TOK_WHITESPACE, lineno, fileno);
      mcc_InsertToken(token, iter);
   }
}

void mcc_DeleteToken(uintptr_t token)
{
   mcc_Token_t *temp = (mcc_Token_t *) token;
   free(temp->text);
   free(temp);
}

void mcc_InsertToken(mcc_Token_t *token, mcc_TokenListIterator_t *iter)
{
   mcc_ListInsertDataAtCurrentPosition((mcc_ListIterator_t *) iter, (uintptr_t) token);

#if MCC_DEBUG
//   printf("Got me a token '%s' of type %d\n", token->text, token->tokenType);
#endif
}

mcc_Token_t *mcc_RemoveCurrentToken(mcc_TokenListIterator_t *iter)
{
   return (mcc_Token_t *) mcc_ListRemoveDataAtCurrentPosition((mcc_ListIterator_t *) iter);
}

const mcc_Token_t *mcc_TokenListPeekCurrentToken(mcc_TokenListIterator_t *iter)
{
   return (const mcc_Token_t *) mcc_ListPeekCurrentData((mcc_ListIterator_t *) iter);
}

void mcc_FreeTokens(void)
{
   mcc_ListDelete(token_list, &mcc_DeleteToken);
   token_list = NULL;
}

void mcc_TokenListDeleteIterator(mcc_TokenListIterator_t *iter)
{
   mcc_ListDeleteIterator((mcc_TokenListIterator_t *) iter);
}

mcc_TokenListIterator_t *mcc_TokenListGetIterator(void)
{
   if (token_list == NULL)
   {
      token_list = mcc_ListCreate();
   }
   return (mcc_TokenListIterator_t *) mcc_ListGetIterator(token_list);
}

mcc_TokenListIterator_t *mcc_TokenListStandaloneGetIterator(mcc_TokenList_t *list)
{
   return (mcc_TokenListIterator_t *) mcc_ListGetIterator(list);
}

mcc_TokenListIterator_t *mcc_TokenListCopyIterator(mcc_TokenListIterator_t *iter)
{
   return (mcc_TokenListIterator_t *) mcc_ListCopyIterator((mcc_ListIterator_t *) iter);
}

mcc_Token_t *mcc_GetNextToken(mcc_TokenListIterator_t *iter)
{
   return (mcc_Token_t *) mcc_ListGetNextData((mcc_ListIterator_t *) iter);
}

mcc_Token_t *mcc_ConCatTokens(mcc_Token_t *first, mcc_Token_t *second, TOKEN_TYPE newType)
{
   int newLength = strlen(first->text) + strlen(second->text);
   //it really only makes sense to be concatenating tokens which are on the same line.
   MCC_ASSERT(first->lineno == second->lineno);
   first->text = (char *) realloc(first->text, newLength);
   strncat(first->text, second->text, newLength);
   first->tokenType = newType;
#if MCC_DEBUG
   printf("Concatenated two tokens to make '%s' of type %d\n",
          first->text, first->tokenType);
#endif
   return first;
}

mcc_TokenList_t *mcc_TokenListCreateStandalone(void)
{
   return (mcc_TokenList_t*) mcc_ListCreate();
}

void mcc_TokenListDeleteStandalone(mcc_TokenList_t *list)
{
   mcc_ListDelete(list, &mcc_DeleteToken);
}

mcc_TokenList_t *mcc_GetTokenList(void)
{
   return token_list;
}

void mcc_WriteTokensToOutputFile(mcc_TokenList_t *tokens)
{
   FILE *outf = fopen(mcc_global_options.outputFilename, "w+");
   mcc_TokenListIterator_t *iter = NULL;
   mcc_Token_t *tok = NULL;
   if (outf == NULL)
   {
      mcc_Error("Couldn't open output file '%s' for writing\n", 
                mcc_global_options.outputFilename);
   }
   iter = mcc_TokenListStandaloneGetIterator(tokens);
   tok = mcc_GetNextToken(iter);
   while (tok != NULL)
   {
      switch (tok->tokenType)
      {
         case TOK_PP_DIRECTIVE:
            fprintf(outf, "#%s", tok->text);
            break;
         case TOK_IDENTIFIER:
         case TOK_KEYWORD:
         case TOK_SYMBOL:
         case TOK_OPERATOR:
         case TOK_STR_CONST:
         case TOK_CHAR_CONST:
         case TOK_NUMBER:
         case TOK_WHITESPACE:
            fprintf(outf, "%s", tok->text);
            break;
         case TOK_SYS_FILE_INC:
            fprintf(outf, "<%s>", tok->text);
            break;
         case TOK_LOCAL_FILE_INC:
            fprintf(outf, "\"%s\"", tok->text);
            break;
         case TOK_EOL:
            fprintf(outf, "\n");
         default:
            break;
      }
      tok = mcc_GetNextToken(iter);
   }
   fclose(outf);
}

#if MCC_DEBUG
void mcc_DebugPrintToken(const mcc_Token_t *token)
{
   if (token != NULL)
   {
      printf("-----Token -----\n\tText: %s\n\tType: %s\n\tIndex: %d\n\tPosition: %s:%d\n",
             token->text, token_types[token->tokenType], token->tokenIndex,
             mcc_ResolveFileNameFromNumber(token->fileno), token->lineno);
   }
   else
   {
      printf("Token was NULL\n");
   }
}
#endif
