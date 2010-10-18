#include <stdio.h>
#include <stdlib.h>

#define FILE_BUFFER_SIZE 32768

void mcc_ParseFile(char *filename)
{
	unsigned char buffer[FILE_BUFFER_SIZE];
	int file = open(filename, O_RDONLY);
	int chars_read = 0;
	int index = 0;
	if (file == NULL)
	{
		mcc_Error("Can't open file %s", filename);
	}

	chars_read = read(file, buffer, FILE_BUFFER_SIZE);
	//
}

int main(int argc, char **argv)
{
	int i;
	for(i = 0; i < argc; i++)
	{
		mcc_ParseFile(argv[i]);
	}
} 
