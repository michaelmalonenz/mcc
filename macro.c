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
#include <string.h>
#include <stdlib.h>

#include "macro.h"
#include "macro_private.h"
#include "mcc.h"


void delete_macro(mcc_Macro_t *macro)
{
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(macro->text != NULL);
   free(macro->text);
   free(macro);
}


// This step should involve macro replacement, so we only have to
// process each macro once - as a side-effect it will correct a bug
// I would have introduced which would define the macro as the latest
// version of it, which is not necessarily the intended one.
// #define SOME_MACRO (3)
// #define SOME_OTHER_MACRO (SOME_MACRO + 4)
// #undef SOME_MACRO
// #define SOME_MACRO (6)
// #define YET_ANOTHER_MACRO (SOME_MACRO + 4)
// If we delayed parsing SOME_OTHER_MACRO until we found each occurence, it 
// would yield the same result as YET_ANOTHER_MACRO, which is bad!
//
// To make life better, I also need to evaluate constant expressions here
mcc_Macro_t *create_macro(const char *text, mcc_TokenList_t *value)
{
   mcc_Macro_t *result = (mcc_Macro_t *) malloc(sizeof(mcc_Macro_t));
   result->text = (char *) malloc(sizeof(char) * (strlen(text) + 1));
   MCC_ASSERT(result != NULL);
   MCC_ASSERT(result->text != NULL);
   strncpy(result->text, text, strlen(text) + 1);
   result->tokens = value;
#if MCC_USE_HASH_TABLE_FOR_MACROS
   result->next = NULL;
#elif MCC_USE_B_TREE_FOR_MACROS
   result->left = NULL;
   result->right = NULL;
#endif
   return result;
}


void mcc_DoMacroReplacement(mcc_LogicalLine_t UNUSED(*line))
{
   // search through the text, and any whole word is potentially
   // a macro.  If it does turn out to be, replace it.
}
