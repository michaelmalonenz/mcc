#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"

/* The order here, is my guess at the relative frequency of each directive's use - so we can match
   earlier in the list, and hopefully speed up the pre-processing portion a little bit */
enum pp_directives { PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF, PP_IF, PP_ENDIF, PP_ELSE,
					 PP_ELIF, PP_UNDEF, PP_ERROR, PP_PRAGMA, NUM_PREPROCESSOR_DIRECTIVES };

static const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
																			"endif", "else", "elif", "undef", "error", "pragma" };


static void searchPreprocessorDirectives(char *line)
{
	int i;
	for(i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
	{
		if (strstr(line, preprocessor_directives[i]) != NULL)
		{
//			printf("%s\n", line);
			return;
		}
	}
}

void mcc_PreprocessFile(const char *inFilename, FILE UNUSED(*outFile))
{
	mcc_FileBuffer_t *fileBuffer = mcc_CreateFileBuffer(inFilename);
	unsigned char *logicalLine = NULL;

	while(!mcc_FileBufferEOFReached(fileBuffer))
	{
		logicalLine = mcc_FileBufferGetNextLogicalLine(fileBuffer);
		printf("%s\n", logicalLine);
		searchPreprocessorDirectives((char *) logicalLine);
		free(logicalLine);
	}

	mcc_DeleteFileBuffer(fileBuffer);
}
