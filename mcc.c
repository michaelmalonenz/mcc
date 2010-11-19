/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcc.h"

//static char **options;

FILE *mcc_OpenFile(const char *filename, char *flags)
{
	FILE *file = NULL;
	file = fopen(filename, flags);
	if (file == NULL)
	{
		mcc_Error("Couldn't open %s\n", filename);
	}
	return file;
}

const char *mcc_FindLocalInclude(const char *filename)
{
	return filename;
}

const char *mcc_FindSystemInclude(const char *filename)
{
	return filename;
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
		mcc_PreprocessFile(argv[i], stdout);
	}

	return 0;
} 
