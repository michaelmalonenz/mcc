#include "config.h"
#include "macro.h"
#include "macro_private.h"

#include <stdint.h>
#include <string.h>

#if MCC_USE_HASH_TABLE_FOR_MACROS

/* I'm told that a x^2 size is good for speed and a hash function
 * which produces uniform results is better than an average hash
 * and a prime number hash table size.
 */
#define HASH_TABLE_LENGTH 8192

static mcc_Macro_t *macro_table[HASH_TABLE_LENGTH];

static uint32_t elf_hash(const void *key, uint16_t len )
{
   const unsigned char *array = key;
   uint32_t hash = 0, mixer;
   uint16_t i;

   for ( i = 0; i < len; i++ ) {
      hash = ( hash << 4 ) + array[i];
      mixer = hash & 0xf0000000L;

      if ( mixer != 0 )
         hash ^= mixer >> 24;

      hash &= ~mixer;
   }
 
   return hash % HASH_TABLE_LENGTH;
}

void mcc_DeleteAllMacros(void)
{
   uint16_t i;
   for (i = 0; i < HASH_TABLE_LENGTH; i++)
   {
      if (macro_table[i] != NULL)
      {
         mcc_Macro_t *current = macro_table[i];
         mcc_Macro_t *next;
         while (current != NULL)
         {
            next = current->next;
            delete_macro(current);
            current = next;
         }
      }
   }
}

void mcc_DefineMacro(const char *text, char *value)
{
   uint32_t hash = elf_hash(text, strlen(text));
   mcc_Macro_t *temp;
   if (macro_table[hash] == NULL)
   {
      macro_table[hash] = create_macro(text, value);
   }
   else
   {
      temp = macro_table[hash];
      while(temp->next != NULL)
         temp = temp->next;
      temp->next = create_macro(text, value);
   }   
}

void mcc_UndefineMacro(const char *text)
{
   mcc_Macro_t *deathRow = mcc_ResolveMacro(text);
   delete_macro(deathRow);
   deathRow = NULL;
}

mcc_Macro_t *mcc_ResolveMacro(const char *text)
{
   mcc_Macro_t *result;
   uint32_t hash = elf_hash(text, (uint16_t) strlen(text));
   result = macro_table[hash];
   while (result != NULL)
   {
      if (memcmp(text, result->text, sizeof(*text)) == 0)
      {
         return result;
      }
      else
      {
         result = result->next;
      }
   }
   return result;
}

#endif
