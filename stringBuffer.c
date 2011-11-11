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
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "stringBuffer.h"

#define MCC_INITIAL_STRING_BUFFER_LENGTH 30

struct StringBuffer {
   char *string;
   unsigned long stringLength;
   unsigned long bufferSize;
};


mcc_StringBuffer_t *mcc_CreateStringBuffer()
{
   mcc_StringBuffer_t *result = (mcc_StringBuffer_t *) malloc(sizeof(mcc_StringBuffer_t));
   MCC_ASSERT(result != NULL);
   result->string = (char *) calloc(sizeof(char), MCC_INITIAL_STRING_BUFFER_LENGTH);
   MCC_ASSERT(result->string != NULL);
   result->bufferSize = MCC_INITIAL_STRING_BUFFER_LENGTH;
   result->stringLength = 0;
   return result;
}

void mcc_DeleteStringBuffer(mcc_StringBuffer_t *buffer)
{
   MCC_ASSERT(buffer != NULL);
   free(buffer->string);
   free(buffer);
}

char *mcc_DestroyBufferNotString(mcc_StringBuffer_t *buffer)
{
   //+1 leaves space for the NUL
   char *string = (char *) realloc(buffer->string,
                                   buffer->stringLength+1);
   if (buffer->stringLength == 0)
   {
      free(string);
      string = NULL;
   }
   MCC_ASSERT(buffer != NULL);
   free(buffer);
   return string;
}

const char *mcc_StringBufferGetString(mcc_StringBuffer_t *buffer)
{
   return (const char *) buffer->string;
}

void mcc_StringBufferAppendChar(mcc_StringBuffer_t *buffer, const char c)
{
   if (buffer->stringLength == buffer->bufferSize)
   {
      buffer->bufferSize *= 2;
      buffer->string = (char *) realloc(buffer->string, buffer->bufferSize);
      /* Is an assert here good enough, or should I error? */
      /* or should I wrap the mem functions with a function that errors clearly when we run out of memory? */
      MCC_ASSERT(buffer->string != NULL);
   }
   buffer->string[buffer->stringLength] = c;
   if (c != '\0')
      buffer->stringLength++;
}

void mcc_StringBufferUnappendChar(mcc_StringBuffer_t *buffer)
{
   buffer->stringLength--;
}

unsigned long mcc_GetStringBufferLength(mcc_StringBuffer_t *buffer)
{
   return buffer->stringLength;
}

int mcc_StringBufferStrncmp(mcc_StringBuffer_t *buffer, const char *string, size_t length)
{
#if MCC_DEBUG
   printf("Comparing: '%s'\nwith\n'%s'\n",
          buffer->string, string);
#endif /* MCC_DBUG */
   return strncmp((const char *)buffer->string, string, length);
}

#if MCC_DEBUG
unsigned long mcc_StringBufferGetBufferSize(mcc_StringBuffer_t *buffer)
{
   return buffer->bufferSize;
}

void mcc_PrintStringBuffer(mcc_StringBuffer_t *buffer)
{
   printf("BufferSize:\t%ld\nString Length:\t%ld\nString:\t'%s'\n",
          buffer->bufferSize, buffer->stringLength, buffer->string);
}
#endif /* MCC_DEBUG */
