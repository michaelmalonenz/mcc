#include <string.h>
#include <stdlib.h>

#include "macro.h"
#include "mcc.h"

/* Do I actually want to turn my binary tree into a hash table?
 * it could be faster, but I don't really know how many macros I'm
 * likely to define in the average case.  TCC uses a hash table
 * for both its macro and symbol table lookup, with a size of 8192
 * (I would actually use a prime number, but you know)
 */
static mcc_Macro_t *root = NULL;

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
static mcc_Macro_t *create_macro(const char *text, const char UNUSED(*value))
{
   mcc_Macro_t *result = (mcc_Macro_t *) malloc(sizeof(mcc_Macro_t));
   result->text = (char *) malloc(sizeof(char) * strlen(text));
   MCC_ASSERT(result != NULL);
   MCC_ASSERT(result->text != NULL);
   strncpy(result->text, text, strlen(text));
   result->left = NULL;
   result->right = NULL;
   return result;
}

static void delete_macro(mcc_Macro_t *macro)
{
   MCC_ASSERT(macro != NULL);
   MCC_ASSERT(macro->text != NULL);
   free(macro->text);
   free(macro);
}


void mcc_DoMacroReplacement(mcc_LogicalLine_t UNUSED(*line))
{
   // search through the text, and any whole word is potentially
   // a macro.  If it does turn out to be, replace it.
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
            current->right = create_macro(text, value);
         else
            current = current->right;
         return;
      }
      else if (cmpResult == 0)
      {
         mcc_Error("Macro '%s' is already defined\n", text);
         return;
      }
      else
      {
         if (current->left == NULL)
            current->left = create_macro(text, value);
         else
            current = current->left;
         return;
      }
   }
}

void mcc_UndefineMacro(const char *text)
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
         //still need to repair the b-tree
         delete_macro(current);
         return;
      }
      else
      {
         current = current->left;
      }
   }
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
