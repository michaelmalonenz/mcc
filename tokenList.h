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
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#ifndef _MCC_TOKEN_LIST_H_
#define _MCC_TOKEN_LIST_H_

#include "config.h"
#include "mcc.h"
#include "list.h"
#include "fileBuffer.h"
#include "tokens.h"

/**
 * It hurts to have to expose this typedef in the header file
 * so please don't abuse it by calling the mcc_List* functions directly!
 */
typedef mcc_ListIterator_t mcc_TokenListIterator_t;
typedef mcc_List_t mcc_TokenList_t;

/**
 * Can't be used with a Standalone token list.
 */
void mcc_FreeTokens(void);

/**
 * Can't be used with a Standalone token list.
 */
mcc_TokenListIterator_t *mcc_TokenListGetIterator(void);

/**
 * This is the global token list, not just any token list.
 */
mcc_TokenList_t *mcc_GetTokenList(void);

#if MCC_DEBUG
void mcc_DebugPrintToken(const mcc_Token_t *token);
void mcc_DebugPrintTokenList(mcc_TokenListIterator_t *iter);
#endif

/**
 * @param text      The physical text of the token as in the source file
 *
 * @param text_len  The length of the physical text
 *
 * @param type      The type of token to create
 *
 * @param lineno    The current line number of the file where the token was found
 * 
 * Allocates the memory for the token and returns it to the caller.  The
 * caller then becomes responsible for freeing the memory.  Calling
 * `mcc_TokenListDeleteStandalone` is sufficient - it is assumed the token
 * will be added to a `mcc_TokenList_t`
 */
mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const unsigned int column,
                             const int lineno, const unsigned short fileno);

/**
 * @param token     The token to copy
 * 
 * @returns a freshly allocated token that contains the content of the original
 * NOTE: does not copy the tokenIndex because I'm lazy.
 */
mcc_Token_t *mcc_CopyToken(const mcc_Token_t *token);

/**
 * @param token - the token to delete
 */
void mcc_DeleteToken(uintptr_t token);

/**
 * @param column The index of the line for the first character
 * @param lineno The line number in the file
 * @param fileno The file identifier
 * @param iter   The iterator defining which collection to add the token to
 *
 * Creates a whitespace token and adds it in the current position.
 */
void mcc_CreateAndAddWhitespaceToken(const unsigned int column,
                                     const int lineno,
                                     const unsigned short fileno,
                                     mcc_TokenListIterator_t *iter);

/**
 * @param column The index of the line for the first character
 * @param lineno The line number in the file
 * @param fileno The file identifier
 * @param iter   The iterator defining which collection to add the token to
 *
 * Creates an end of Line token and adds it in the current position.
 */
void mcc_AddEndOfLineToken(const unsigned int column, const int lineno, const unsigned short fileno,
                           mcc_TokenListIterator_t *iter);

void mcc_InsertToken(mcc_Token_t *token, mcc_TokenListIterator_t *iter);
const mcc_Token_t *mcc_TokenListPeekCurrentToken(mcc_TokenListIterator_t *iter);


mcc_TokenList_t *mcc_TokenListCreateStandalone(void);
void mcc_TokenListDeleteStandalone(mcc_TokenList_t *list);
mcc_TokenListIterator_t *mcc_TokenListStandaloneGetIterator(mcc_TokenList_t *list);

/**
 * Add a token to the end of a list.
 * Instead of requiring an interator to insert at a particular position,
 * just append to the end of the list.  No one needs to deal with an iterator's
 * memory that way.
 * 
 * @param list The list to append to
 * @param token The token to append to the list
 * */
void mcc_TokenListStandaloneAppend(mcc_TokenList_t *list, mcc_Token_t *token);


/**
 * Deep copies the list - the caller becomes responsible for freeing
 * both the list and the tokens in it.
 *
 * @param list - The list to copy
 */
mcc_List_t *mcc_TokenListDeepCopy(mcc_TokenList_t *list);

/**
 * Replace the current token as defined by the iterator with the given
 * token.
 */
void mcc_TokenListStandaloneReplaceCurrent(mcc_TokenListIterator_t *iter, mcc_Token_t *token);

mcc_Token_t *mcc_ConCatTokens(mcc_Token_t *first, mcc_Token_t *second, TOKEN_TYPE newType);
mcc_TokenListIterator_t *mcc_TokenListCopyIterator(mcc_TokenListIterator_t *iter);
void mcc_TokenListDeleteIterator(mcc_TokenListIterator_t *iter);
mcc_Token_t *mcc_GetNextToken(mcc_TokenListIterator_t *iter);

void mcc_WriteTokensToOutputFile(mcc_TokenList_t *tokens);

void shunt(mcc_TokenListIterator_t *iter);

#endif /* _MCC_TOKEN_LIST_H_ */

