/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include "mcc.h"

//static char **input_files;
//static char **options;

int main(int argc, char **argv)
{
	int i;
	for(i = 1; i < argc; i++)
	{
		mcc_ParseFile(argv[i]);
	}

	return 0;
} 
