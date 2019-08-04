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

#define MCC_DEBUG 1
#include "mcc.h"
#include "stringBuffer.h"


static void test_Allocation(void)
{
	eral_StringBuffer_t *buffer = eral_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(eral_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(eral_GetStringBufferLength(buffer) == 0);
	eral_DeleteStringBuffer(buffer);
}

static void test_Append(void)
{
	const char *string = "This is a short string";
	const char *ptr = string;
	eral_StringBuffer_t *buffer = eral_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(eral_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(eral_GetStringBufferLength(buffer) == 0);
	
	do
	{
           eral_StringBufferAppendChar(buffer, *ptr);
	}while(*ptr++);
	eral_StringBufferAppendChar(buffer, '\0');
	eral_PrintStringBuffer(buffer);
	MCC_ASSERT(eral_StringBufferStrncmp(buffer, string, eral_GetStringBufferLength(buffer)) == 0);
	MCC_ASSERT(eral_GetStringBufferLength(buffer) == strlen(string)); //buffer has an extra for '\0'
	eral_DeleteStringBuffer(buffer);
}

static void test_AppendWithResize(void)
{
	const char *string = "This is an exceedingly long string and it goes on and on my friend.  Some people, started writing it, not knowing what it was, and they'll continue stringing it along if only just because...";
	const char *ptr = string;
	eral_StringBuffer_t *buffer = eral_CreateStringBuffer();
	MCC_ASSERT(buffer != NULL);
	MCC_ASSERT(eral_StringBufferGetBufferSize(buffer) > 0);
	MCC_ASSERT(eral_GetStringBufferLength(buffer) == 0);
	
	do
	{
		eral_StringBufferAppendChar(buffer, *ptr);
	}while(*ptr++);
	eral_StringBufferAppendChar(buffer, '\0');
	eral_PrintStringBuffer(buffer);
	MCC_ASSERT(eral_StringBufferStrncmp(buffer, string, eral_GetStringBufferLength(buffer)) == 0);
	MCC_ASSERT(eral_GetStringBufferLength(buffer) == strlen(string));
	eral_DeleteStringBuffer(buffer);
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

