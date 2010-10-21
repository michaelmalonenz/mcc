#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "tokens.h"
#include "mcc.h"

#define FILE_BUFFER_SIZE 32768
#define MAX_SYMBOL_LENGTH 512

static bool_t isWhiteSpace(char c)
{
	return (c == '\t' || c == '\v' ||
			c == '\r' || c == '\n' ||
			c == '\v' || c == ' ');
}

void mcc_ParseFile(const char *filename)
{
	unsigned char buffer[FILE_BUFFER_SIZE];
	int file = open(filename, O_RDONLY);
	unsigned char current_token[MAX_SYMBOL_LENGTH];
	int current_token_index = 0;
	int index = 0;
	int chars_read;
	if (file == 0)
	{
		mcc_Error("Can't open file '%s' for reading\n", filename);
	}

	chars_read = read(file, buffer, FILE_BUFFER_SIZE);
	while(chars_read > 0)
	{
		for(index = 0; index < chars_read; index++)
		{
			if (current_token_index == MAX_SYMBOL_LENGTH)
			{
				current_token[MAX_SYMBOL_LENGTH - 1] = '\0';
				mcc_Error("Error parsing symbol '%s' in file '%s'\n", current_token, filename);
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
				current_token[current_token_index++] = buffer[index];
			}
		}
		chars_read = read(file, buffer, FILE_BUFFER_SIZE);
	}
}

