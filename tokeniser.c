#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "mcc.h"

#if MCC_USE_DYNAMIC_STRING_BUFFER_TOKENISER
#include "stringBuffer.h"
#endif

#define MAX_SYMBOL_LENGTH 512

/* There are two distinct implementations of this function.  One which uses dynamic
 * and the other static strings.  If static strings aren't significantly faster,
 * I'll probably end up throwing that implementation away, but I need both to compare
 * 
 * Should probably return the number of unused chars which didn't contain a whole
 * token.
 */
void mcc_TokeniseString(const char *string, const unsigned int length)
#if MCC_USE_DYNAMIC_STRING_BUFFER_TOKENISER
{
	mcc_StringBuffer_t *current_token = mcc_CreateStringBuffer();
	unsigned int index = 0;

	for(index = 0; index < length; index++)
	{
		if (isWhiteSpace(string[index]))
		{
			
		}
		else
		{
			mcc_StringBufferAppendChar(current_token, string[index]);
		}
	}
}
#else
{
	signed char current_token[MAX_SYMBOL_LENGTH];
	int current_token_index = 0;
	int index = 0;

	for(index = 0; index < length; index++)
	{
		if (current_token_index == MAX_SYMBOL_LENGTH)
		{
			current_token[MAX_SYMBOL_LENGTH - 1] = '\0';
			mcc_Error("Error parsing symbol '%s' in file '%s'\nToken is too long", current_token, filename);
		}
		/* if we've found some white space, we've probably got a token in the buffer,
		   figure out what it is and throw it in the list */
		if (isWhiteSpace(buffer[index]))
		{
			/* ignore contiguous whitespace */
			if (current_token_index == 0)
				continue;
			current_token_index = 0;
			/* does this symbol start with a number?
			   search through the symbols, then the keywords, then assume it's an identifier */
		}
		else
		{
			int i,j;
			for(i = 0; i < NUM_OPERATORS; i++)
			{
				if (strncmp((const char *)current_token, operators[i],
							current_token_index) == 0)
				{
					current_token[current_token_index] = buffer[index];
					for(j = 0; j < NUM_OPERATORS; j++)
					{
						if (strncmp((const char *)current_token, operators[j], current_token_index + 1) == 0)
						{
						}
					}								
				}
			}
			current_token[current_token_index++] = buffer[index];
		}
	}
}

#endif
