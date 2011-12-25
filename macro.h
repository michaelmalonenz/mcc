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
#ifndef MCC_MACRO_H
#define MCC_MACRO_H

#include "fileBuffer.h"
#include "config.h"
#include "tokenList.h"

typedef struct macro {
   char *text;
   char *value;
   mcc_TokenList_t *tokens;
#if MCC_USE_HASH_TABLE_FOR_MACROS
   struct macro *next;
#elif MCC_USE_B_TREE_FOR_MACROS
   struct macro *left;
   struct macro *right;
#endif
}mcc_Macro_t;

void mcc_DeleteAllMacros(void);

void mcc_DefineMacro(const char *text, mcc_TokenList_t *tokens);

void mcc_UndefineMacro(const char *text);

mcc_Macro_t *mcc_ResolveMacro(const char *text);

void mcc_DoMacroReplacement(mcc_LogicalLine_t *line);

#endif /* MCC_MACRO_H */
