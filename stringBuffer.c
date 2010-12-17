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
   char *string = (char *) realloc(buffer->string,
                                   buffer->stringLength);
   if (buffer->stringLength == 0)
   {
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
      buffer->string = (char *) realloc(buffer->string, buffer->bufferSize * 2);
      buffer->bufferSize *= 2;
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
   //Should I also clobber the last char?
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
