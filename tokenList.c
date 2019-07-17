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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "options.h"
#include "tokens.h"
#include "tokenList.h"
#include "liberal.h"
#include "mcc.h"

static mcc_TokenList_t *token_list = NULL;

static const char whitespaceText = ' ';

mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, int token_index,
                             const unsigned int column,
                             const int lineno, const unsigned short fileno)
{
   mcc_Token_t *token = (mcc_Token_t *) malloc(sizeof(mcc_Token_t));
   token->text = (char *) malloc(sizeof(char) * (text_len + 1));
   memcpy(token->text, text, text_len);
   token->text[text_len] = '\0';
   token->tokenType = type;
   token->tokenIndex = token_index;
   token->line_index = column;
   token->lineno = lineno;
   token->fileno = fileno;

   return token;
}

mcc_Token_t *mcc_CopyToken(const mcc_Token_t *token)
{
   mcc_Token_t *result = mcc_CreateToken(
      token->text, strlen(token->text), token->tokenType,
      token->tokenIndex, token->line_index, token->lineno, token->fileno);

   if (result->tokenType == TOK_NUMBER)
   {
      result->number = token->number;
   }
   return result;
}

mcc_Token_t *mcc_CreateNumberToken(mcc_Number_t *number,
   const unsigned int column, const int lineno, const unsigned short fileno)
{
   char numberText[20] = {0};
   snprintf(numberText, 20, "%d", number->number.integer_s);
   mcc_Token_t *result = mcc_CreateToken(
      numberText, strlen(numberText), TOK_NUMBER, TOK_UNSET_INDEX,
      column, lineno, fileno);
   memcpy(&result->number, number, sizeof(*number));
   return result;
}

mcc_List_t *mcc_TokenListDeepCopy(mcc_TokenList_t *list)
{
   mcc_List_t *result = mcc_ListCreate();
   mcc_TokenListIterator_t *iter = mcc_TokenListStandaloneGetIterator(list);
   mcc_Token_t *token = mcc_GetNextToken(iter);
   while (token != NULL)
   {
      mcc_TokenListStandaloneAppend(result, mcc_CopyToken(token));
      token = mcc_GetNextToken(iter);
   }
   mcc_TokenListDeleteIterator(iter);
   return result;
}

void mcc_AddEndOfLineToken(const unsigned int column, const int lineno, const unsigned short fileno,
                           mcc_TokenListIterator_t *iter)
{
   mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                        TOK_EOL, TOK_UNSET_INDEX, column, lineno, fileno);
   mcc_InsertToken(token, iter);   
}

void mcc_CreateAndAddWhitespaceToken(const unsigned int column, const int lineno,
                                     const unsigned short fileno,
                                     mcc_TokenListIterator_t *iter)
{
   const mcc_Token_t *temp = mcc_TokenListPeekCurrentToken(iter);
   if (temp != NULL && temp->tokenType != TOK_WHITESPACE)
   {
      mcc_Token_t *token = mcc_CreateToken(&whitespaceText, sizeof(whitespaceText),
                                           TOK_WHITESPACE, TOK_UNSET_INDEX, column,
                                           lineno, fileno);
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
}

mcc_Token_t *mcc_TokenListStandaloneReplaceCurrent(mcc_TokenListIterator_t *iter, mcc_Token_t *token)
{
   return (mcc_Token_t *)mcc_ListReplaceCurrentData((mcc_ListIterator_t *)iter, (uintptr_t)token);
}

const mcc_Token_t *mcc_TokenListPeekCurrentToken(mcc_TokenListIterator_t *iter)
{
   return (const mcc_Token_t *) mcc_ListPeekCurrentData((mcc_ListIterator_t *) iter);
}

const mcc_Token_t *mcc_TokenListPeekNextToken(mcc_TokenListIterator_t *iter)
{
   return (const mcc_Token_t *) mcc_ListPeekNextData((mcc_ListIterator_t *)iter);
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

void mcc_TokenListStandaloneAppend(mcc_TokenList_t *list, mcc_Token_t *token)
{
   mcc_ListAppendData((mcc_List_t *)list, (uintptr_t)token);
}

mcc_TokenListIterator_t *mcc_TokenListCopyIterator(mcc_TokenListIterator_t *iter)
{
   return (mcc_TokenListIterator_t *) mcc_ListCopyIterator((mcc_ListIterator_t *) iter);
}

mcc_Token_t *mcc_GetNextToken(mcc_TokenListIterator_t *iter)
{
   return (mcc_Token_t *) mcc_ListGetNextData((mcc_ListIterator_t *) iter);
}

const mcc_Token_t *mcc_PeekPreviousNonWhitespaceToken(mcc_TokenListIterator_t *iter)
{
   const mcc_Token_t *tok = (mcc_Token_t *) mcc_ListPeekCurrentData(iter);
   if (tok && tok->tokenType != TOK_WHITESPACE)
      return tok;
   mcc_TokenListIterator_t *temp = mcc_ListCopyIterator(iter);
   while (tok && tok->tokenType == TOK_WHITESPACE)
      tok = (mcc_Token_t *) mcc_ListGetPrevData(temp);
   mcc_ListDeleteIterator(temp);
   return tok;
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
      printf("\t----- Token -----\n\tText: %s\n\tType: %s\n\tIndex: %d\n\tPosition: %s:%d:%u\n",
             token->text, token_types[token->tokenType], token->tokenIndex,
             mcc_ResolveFileNameFromNumber(token->fileno), token->lineno, token->line_index);
   }
   else
   {
      printf("Token was NULL\n");
   }
}

void mcc_DebugPrintTokenList(mcc_TokenListIterator_t *iter)
{
   mcc_TokenListIterator_t *copy = mcc_TokenListCopyIterator(iter);
   mcc_Token_t *token = (mcc_Token_t *)mcc_TokenListPeekCurrentToken(copy);
   printf("----- Token List -----\n");
   while (token != NULL)
   {
      mcc_DebugPrintToken(token);
      token = mcc_GetNextToken(copy);
   }
   printf("----- End Token List -----\n");
   mcc_TokenListDeleteIterator(copy);
}
#endif
