/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include "mcc.h"
#include <string.h>
#include <stdlib.h>

static char *input_files[MCC_MAX_INPUT_FILES];
static unsigned short num_files = 0;

//static char **options;

int main(int argc, char **argv)
{
	int i;
	for(i = 1; i < argc; i++)
	{
		input_files[i-1] = (char *) malloc(sizeof(char) * strlen(argv[i]));
		strncpy(input_files[i-1], argv[i], strlen(argv[i]));
		num_files++;
	}

	for(i = 0; i < num_files; i++)
	{
		mcc_ParseFile(input_files[i]);
	}

	return 0;
} 
