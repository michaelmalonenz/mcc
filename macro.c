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
#include <string.h>
#include <stdlib.h>

#include "macro.h"
#include "macro_private.h"
#include "mcc.h"
#include "tokenList.h"


void delete_macro(mcc_Macro_t *macro)
{
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(macro->text != NULL);
   if (macro->arguments != NULL)
   {
      mcc_TokenListDeleteStandalone(macro->arguments);
   }
   if (macro->tokens != NULL)
   {
      mcc_TokenListDeleteStandalone(macro->tokens);
   }
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
mcc_Macro_t *create_macro(const char *text, mcc_TokenList_t *value, mcc_TokenList_t *arguments)
{
   mcc_Macro_t *result = (mcc_Macro_t *) malloc(sizeof(mcc_Macro_t));
   result->text = (char *) malloc(sizeof(char) * (strlen(text) + 1));
   MCC_ASSERT(result != NULL);
   MCC_ASSERT(result->text != NULL);
   strncpy(result->text, text, strlen(text) + 1);
   result->tokens = value;
   result->arguments = arguments;
   result->is_function = (bool_t)(arguments != NULL);
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

bool_t mcc_IsMacroDefined(const char *text)
{
   return (mcc_ResolveMacro(text) != NULL);
}
