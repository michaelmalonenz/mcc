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
void mcc_DefineMacro(const char *text, char *value)
{
   mcc_Macro_t *current = root;
   if (root == NULL)
   {
      root = create_macro(text, value);
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
            current->right = create_macro(text, value);
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
            current->left = create_macro(text, value);
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
         else
         {
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
