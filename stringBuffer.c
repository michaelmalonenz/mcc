#include <stdlib.h>
#include "mcc.h"
#include "stringBuffer.h"

#define MCC_INITIAL_STRING_BUFFER_LENGTH 30

struct StringBuffer {
	signed char *string;
	unsigned int stringLength;
	unsigned int bufferSize;
};


mcc_StringBuffer_t *mcc_CreateStringBuffer()
{
	mcc_StringBuffer_t *result = (mcc_StringBuffer_t *) malloc(sizeof(mcc_StringBuffer_t));
	MCC_ASSERT(result != NULL);
	result->string = (signed char *) malloc(sizeof(signed char) *MCC_INITIAL_STRING_BUFFER_LENGTH);
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

void mcc_AppendChar(mcc_StringBuffer_t *buffer, const signed char c)
{
	if (buffer->stringLength == buffer->bufferSize)
	{
		buffer->string = (signed char *) realloc(buffer->string, buffer->bufferSize * 2);
		buffer->bufferSize *= 2;
		MCC_ASSERT(buffer->string != NULL);
	}
	buffer->string[buffer->stringLength++] = c;
}

unsigned int mcc_GetStringBufferLength(mcc_StringBuffer_t *buffer)
{
	return buffer->stringLength;
}

