#include <stdio.h>
#include <stdlib.h>

#include "mcc.h"

/* A good, meaty base-2 chunk of a file, so we don't start reading the thing
 * from disk a character at a time
 */
#define FILE_BUFFER_SIZE 32768

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

/* I need a better search structure than a list for the macros, but given that I don't
 * have _any_ idea how many values I will end up having (and especially since re-sizing
 * would be costly, am I going to have to settle for a b-tree?
 */



void mcc_PreprocessFile(FILE *inFile, FILE UNUSED(*outFile))
{
	mcc_FileBuffer_t *fileBuffer = (mcc_FileBuffer_t *) malloc(sizeof(mcc_FileBuffer_t));
	fileBuffer->file = inFile;
	fileBuffer->line_no = 0;
	fileBuffer->bufferIndex = 0;
	fileBuffer->chars_read = fread(fileBuffer->buffer,
								   sizeof(*(fileBuffer->buffer)),
								   FILE_BUFFER_SIZE,
								   fileBuffer->file);
	fileBuffer->buffer[fileBuffer->chars_read + 1] = '\0'; //make life a little easier for ourselves
	/* preprocessor directives:
	 * #define
	 * #undef
	 * #define(xxx) //function-ish macros; do I need to handle variadic macros separately?
	 * #if
	 * #ifdef
	 * #ifndef
	 * #elif
	 * #else
	 * #endif
	 * #error
	 * #pragma
	 * #include
	 */
}
