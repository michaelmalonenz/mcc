#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "mcc.h"

#define FILE_BUFFER_SIZE 32768

void mcc_ParseFile(const char *filename)
{
	unsigned char buffer[FILE_BUFFER_SIZE];
	int file = open(filename, O_RDONLY);
	int chars_read = 0;
//	int index = 0;
	if (file == 0)
	{
		mcc_Error("Can't open file %s", filename);
	}

	chars_read = read(file, buffer, FILE_BUFFER_SIZE);
	//
}

