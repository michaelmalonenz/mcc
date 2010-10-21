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

	if (argc <= 1)
	{
		mcc_Error("No input files\n");
	}

	for(i = 1; i < argc; i++)
	{
		input_files[i-1] = (char *) malloc(sizeof(char) * strlen(argv[i]));
		strncpy(input_files[i-1], argv[i], strlen(argv[i]));
		num_files++;
		mcc_ParseFile(input_files[i]);
	}


	for(i = 0; i < num_files; i++)
	{
		free(input_files[i]);
	}

	return 0;
} 
