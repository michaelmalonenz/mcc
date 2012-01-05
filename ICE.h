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
#ifndef _MCC_ICE_H_
#define _MCC_ICE_H_

#include "tokenList.h"

/**
 * @brief An Integer Constant Expression is a compile-time expression which
 * results in an integer.  This function takes a string of tokens which make up
 * a mathmatical expression and returns the evaluated result.
 *
 * @param iter - an iterator which will return the entire expression token string
 *               and nothing more.
 *
 * @returns the evaluated result of the expression
 *
 * @remarks If there is a token which doesn't make up and ICE, the function will
 *          error and terminate execution of the program.
 */
int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter);

#endif /* _MCC_ICE_H_ */
