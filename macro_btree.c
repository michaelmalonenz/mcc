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

#include "macro.h"
#include "macro_private.h"


#if MCC_USE_B_TREE_FOR_MACROS

static mcc_Macro_t *root = NULL;


/** This whole implementation is pretty crappy. It has too much duplication,
 *  the functions are too ad-hoc and it feels generally fragile.
 */


static void delete_macroTree(mcc_Macro_t *root)
{
   if (root->left != NULL)
   {
      delete_macroTree(root->left);
      root->left = NULL;
   }
   if (root->right != NULL)
   {
      delete_macroTree(root->right);
      root->right = NULL;
   }
   delete_macro(root);
   root = NULL;
}

void mcc_DeleteAllMacros(void)
{
   delete_macroTree(root);
}

// I should really refactor these next couple of functions
// because they're somewhat similar
void mcc_DefineMacro(const char *text, mcc_TokenList_t *tokens)
{
   mcc_Macro_t *current = root;
   if (root == NULL)
   {
      root = create_macro(text, tokens);
      return;
   }

   while (current != NULL)
   {
      int cmpResult = strncmp(text, current->text, 
                              max(strlen(current->text), strlen(text)));
      if (cmpResult > 0)
      {
         if (current->right == NULL)
         {
            current->right = create_macro(text, tokens);
            return;
         }
         else
         {
            current = current->right;
         }
      }
      else if (cmpResult == 0)
      {
         mcc_Error("Macro '%s' is already defined\n", text);
         return;
      }
      else
      {
         if (current->left == NULL)
         {
            current->left = create_macro(text, tokens);
            return;
         }
         else
         {
            current = current->left;
         }
      }
   }
}

void mcc_UndefineMacro(const char *text)
{
   mcc_Macro_t *current = root;
   mcc_Macro_t *last = NULL;
   while (current != NULL)
   {
      int cmpResult = strncmp(text, current->text, 
                              max(strlen(current->text), strlen(text)));
      last = current;

      if (cmpResult > 0)
      {
         current = current->right;
      }
      else if (cmpResult == 0)
      {
         bool_t nullify = (current == root);
         delete_macro(current);
         if (nullify)
         {
            root = NULL;
         }
         return;
      }
      else
      {
         current = current->left;
      }
   }
   //the spec says we need to silently ignore undef directives when the macro isn't previously defined.
   //However, I would like to have an option to issue a warning here.
}

mcc_Macro_t *mcc_ResolveMacro(const char *text)
{
   mcc_Macro_t *current = root;
   while (current != NULL)
   {
      int cmpResult = strncmp(text, current->text, 
                              max(strlen(current->text), strlen(text)));
      if (cmpResult > 0)
      {
         current = current->right;
      }
      else if (cmpResult == 0)
      {
         return current;
      }
      else
      {
         current = current->left;
      }
   }
   return NULL;
}

#endif /* MCC_USE_HASH_TABLE_FOR_MACROS */
