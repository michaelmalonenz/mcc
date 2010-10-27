#include <string.h>
#include "mcc.h"
#include "stringBuffer.h"


static void test_Allocation(void)
{
	mcc_StringBuffer_t *buffer = mcc_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(mcc_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(mcc_GetStringBufferLength(buffer) == 0);
	mcc_DeleteStringBuffer(buffer);
}

static void test_Append(void)
{
	const char *string = "This is a short string";
	const char *ptr = string;
	mcc_StringBuffer_t *buffer = mcc_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(mcc_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(mcc_GetStringBufferLength(buffer) == 0);
	
	do
	{
		mcc_StringBufferAppendChar(buffer, *ptr);
	}while(*ptr++);
	mcc_StringBufferAppendChar(buffer, '\0');
	mcc_PrintStringBuffer(buffer);
	MCC_ASSERT(mcc_StringBufferStrncmp(buffer, string, mcc_GetStringBufferLength(buffer)) == 0);
	MCC_ASSERT(mcc_GetStringBufferLength(buffer) == strlen(string));
	mcc_DeleteStringBuffer(buffer);
}

static void test_AppendWithResize(void)
{
	const char *string = "This is an exceedingly long string and it goes on and on my friend.  Some people, started writing it, not knowing what it was, and they'll continue stringing it along if only just because...";
	const char *ptr = string;
	mcc_StringBuffer_t *buffer = mcc_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(mcc_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(mcc_GetStringBufferLength(buffer) == 0);
	
	do
	{
		mcc_StringBufferAppendChar(buffer, *ptr);
	}while(*ptr++);
	mcc_StringBufferAppendChar(buffer, '\0');
	mcc_PrintStringBuffer(buffer);
	MCC_ASSERT(mcc_StringBufferStrncmp(buffer, string, mcc_GetStringBufferLength(buffer)) == 0);
	MCC_ASSERT(mcc_GetStringBufferLength(buffer) == strlen(string));
	mcc_DeleteStringBuffer(buffer);
}


/* I shouldn't care about the internals - i.e. don't test when it resizes exactly,
 * only that it handles largish strings and tells me the right size
 */
int main(void)
{
	test_Allocation();
	test_Append();
	test_AppendWithResize();
	return 0;
}

