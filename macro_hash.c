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
   uint32_t hash = elf_hash(text, (uint16_t) strlen(text));
   mcc_Macro_t *previous,*current;
   current = previous = macro_table[hash];
   while(current != NULL)
   {
      if (memcmp(text, current->text, sizeof(*text)) == 0)
      {
         /* This means that we're at the head of the list */
         if (previous == current)
         {
            macro_table[hash] = current->next;
         }
         else
         {
            previous->next = current->next;
         }
         delete_macro(current);
         return;
      }
      else
      {
         previous = current;
         current = current->next;
      }
   }
}

mcc_Macro_t *mcc_ResolveMacro(const char *text)
{
   uint32_t hash = elf_hash(text, (uint16_t) strlen(text));
   mcc_Macro_t *result = macro_table[hash];

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
