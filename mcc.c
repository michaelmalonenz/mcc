/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "mcc.h"

static char *input_files[MCC_MAX_INPUT_FILES];
static unsigned short num_files = 0;

//static char **options;

int mcc_OpenFile(const char *filename, unsigned int flags)
{
	int fd;
	fd  = open(filename, flags);
	if (fd == 0)
	{
		mcc_Error("Couldn't open %s", filename);
	}
	return fd;
}


int main(int argc, char **argv)
{
	int i;

	if (argc <= 1)
	{
		mcc_Error("No input files\n");
	}

	for(i = 1; i < argc; i++)
	{
		input_files[i-1] = &argv[i];
		num_files++;
		mcc_PreprocessFile(mcc_OpenFile(input_files[i-1], O_RDONLY), stdout);
	}


	for(i = 0; i < num_files; i++)
	{
		free(input_files[i]);
	}

	return 0;
} 
