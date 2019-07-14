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

void mcc_StringBufferAppendString(mcc_StringBuffer_t *buffer, const char *string)
{
   int len = strlen(string);
   int i;
   for (i = 0; i < len; i++)
   {
      mcc_StringBufferAppendChar(buffer, string[i]);
   }
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
