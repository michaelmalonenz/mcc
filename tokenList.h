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
 * @param text      The physical text of the token as in the source file
 *
 * @param text_len  The length of the physical text
 *
 * @param type      The type of token to create
 *
 * @param lineno    The current line number of the file where the token was found
 */
mcc_Token_t *mcc_CreateToken(const char *text, size_t text_len,
                             TOKEN_TYPE type, const int lineno,
                             const unsigned short fileno);

void mcc_DeleteToken(void *token);

/**
 * @param lineno The line number in the file
 *
 * Creates a whitespace token and adds it in the current position.
 */
void mcc_CreateAndAddWhitespaceToken(const int lineno,
                                     const unsigned short fileno,
                                     mcc_TokenListIterator_t *iter);

/**
 * @param lineno The line number in the file
 *
 * Creates an end of Line token and adds it in the current position.
 */
void mcc_AddEndOfLineToken(const int lineno, const unsigned short fileno,
                           mcc_TokenListIterator_t *iter);

void mcc_InsertToken(mcc_Token_t *token, mcc_TokenListIterator_t *iter);
mcc_Token_t *mcc_RemoveCurrentToken(mcc_TokenListIterator_t *iter);
const mcc_Token_t *mcc_TokenListPeekCurrentToken(mcc_TokenListIterator_t *iter);

void mcc_FreeTokens(void);
mcc_Token_t *mcc_ConCatTokens(mcc_Token_t *first, mcc_Token_t *second, TOKEN_TYPE newType);
mcc_TokenListIterator_t *mcc_TokenListCopyIterator(mcc_TokenListIterator_t *iter);
mcc_TokenListIterator_t *mcc_TokenListGetIterator(void);
void mcc_TokenListDeleteIterator(mcc_TokenListIterator_t *iter);
mcc_Token_t *mcc_GetNextToken(mcc_TokenListIterator_t *iter);


#if MCC_DEBUG
mcc_TokenList_t *mcc_DebugGetTokenList(void);
#endif

#endif /* _MCC_TOKEN_LIST_H_ */

