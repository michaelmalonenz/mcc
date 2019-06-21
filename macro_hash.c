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
#include "config.h"
#include "macro.h"
#include "macro_private.h"

#include <stdint.h>
#include <string.h>

/* I'm told that a x^2 size is good for speed and a hash function
 * which produces uniform results is better than an average hash
 * and a prime number hash table size.
 */
#define HASH_TABLE_LENGTH 8192

static mcc_Macro_t *macro_table[HASH_TABLE_LENGTH];

static uint32_t elf_hash(const void *key, uint16_t len)
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

void mcc_InitialiseMacros(void)
{
   int i;
   for (i = 0; i < HASH_TABLE_LENGTH; i++)
   {
      macro_table[i] = NULL;
   }
   mcc_DefineMacro("__STDC__", NULL, NULL);
#if MCC_C99_COMPATIBLE
   mcc_TokenList_t *value = mcc_TokenListCreateStandalone();
   mcc_Number_t number;
   number.number.integer_s = 199901L;
   number.numberType = SIGNED_INT;
   mcc_TokenListStandaloneAppend(value, mcc_CreateNumberToken(&number, 0, 0, 255));
   mcc_DefineMacro("__STDC_VERSION__", value, NULL);
#endif
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
         macro_table[i] = NULL;
      }
   }
}

void mcc_DefineMacro(const char *text, mcc_TokenList_t *value, mcc_TokenList_t *arguments)
{
   uint32_t hash = elf_hash(text, strlen(text));
   mcc_Macro_t *temp;
   if (macro_table[hash] == NULL)
   {
      macro_table[hash] = create_macro(text, value, arguments);
   }
   else
   {
      temp = macro_table[hash];
      while(temp->next != NULL)
         temp = temp->next;
      temp->next = create_macro(text, value, arguments);
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
      if (strncmp(text, result->text, max(strlen(result->text), strlen(text))) == 0)
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
