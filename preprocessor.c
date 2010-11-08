#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "stringBuffer.h"

/* A good, meaty base-2 chunk of a file, so we don't start reading the thing
 * from disk a character at a time
 */
#define FILE_BUFFER_SIZE 32768

#define TAB_REPLACEMENT_SPACES "    "

/* Should avoid allocating these on the stack - it could use up memory much too quickly
 * (stacks are usually limited to around 8MB) Which, I suppose is big enough to hold a 
 * decent amount, but don't I want to leave plenty of room for other stuff?
 */
typedef struct FileBuffer {
	FILE *file;
	char *filename;
	unsigned int line_no;
	unsigned int bufferIndex;
	unsigned char buffer[FILE_BUFFER_SIZE + 1];
	unsigned char chars_read;
} mcc_FileBuffer_t;

/* The order here, is my guess at the relative frequency of each directive's use - so we can match
   earlier in the list, and hopefully speed up the pre-processing portion a little bit */
enum pp_directives { PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF, PP_IF, PP_ENDIF, PP_ELSE,
					 PP_ELIF, PP_UNDEF, PP_ERROR, PP_PRAGMA, NUM_PREPROCESSOR_DIRECTIVES };

static const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
																			"endif", "else", "elif", "undef", "error", "pragma" };

static void readFileChunk(mcc_FileBuffer_t *fileBuffer)
{
	fileBuffer->chars_read = fread(fileBuffer->buffer,
								   sizeof(*(fileBuffer->buffer)),
								   FILE_BUFFER_SIZE,
								   fileBuffer->file);
	fileBuffer->buffer[fileBuffer->chars_read + 1] = '\0'; //make life a little easier for ourselves
}

static unsigned char *getNextLogicalLine(mcc_FileBuffer_t *fileBuffer)
{
	mcc_StringBuffer_t *lineBuffer = mcc_CreateStringBuffer();
	while (fileBuffer->bufferIndex < fileBuffer->chars_read)
	{
	}
	fileBuffer->line_no++;
	return mcc_DestroyBufferNotString(lineBuffer);
}

static void searchPreprocessorDirectives(char *line)
{
	int i;
	for(i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
	{
		if (strstr(line, preprocessor_directives[i]) != NULL)
			return;
	}
}

void mcc_PreprocessFile(FILE *inFile, FILE UNUSED(*outFile))
{
	mcc_FileBuffer_t *fileBuffer = (mcc_FileBuffer_t *) malloc(sizeof(mcc_FileBuffer_t));
	unsigned char *logicalLine = NULL;
	fileBuffer->file = inFile;
	fileBuffer->line_no = 0;
	fileBuffer->bufferIndex = 0;
	readFileChunk(fileBuffer);

	while(fileBuffer->chars_read > 0)
	{
		logicalLine = getNextLogicalLine(fileBuffer);
		searchPreprocessorDirectives((char *) logicalLine);
	}
}
