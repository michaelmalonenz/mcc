#include <errno.h>
#include <string.h>

#include "fileBuffer.h"
#include "stringBuffer.h"
#include "mcc.h"

/* A good, meaty base-2 chunk of a file, so we don't start reading the thing
 * from disk a character at a time
 */
#define FILE_BUFFER_SIZE 32768

#define TAB_REPLACEMENT_SPACES "    "

/* Should avoid allocating these on the stack - it could use up memory much too quickly
 * (stacks are usually limited to around 8MB) Which, I suppose is big enough to hold a 
 * decent amount, but don't I want to leave plenty of room for other stuff?
 */
struct FileBuffer {
	FILE *file;
	const char *filename;
	unsigned int line_no;
	unsigned int bufferIndex;
	unsigned char buffer[FILE_BUFFER_SIZE + 1];
	size_t chars_read;
    mcc_LogicalLine_t currentLine;
};


#if MCC_DEBUG
void printFileBuffer(mcc_FileBuffer_t *buffer);
#endif

const char *mcc_GetFileBufferFilename(mcc_FileBuffer_t *fileBuffer)
{
	return fileBuffer->filename;
}

unsigned int mcc_GetFileBufferCurrentLineNo(mcc_FileBuffer_t *fileBuffer)
{
	return fileBuffer->line_no;
}

mcc_FileBuffer_t *mcc_CreateFileBuffer(const char *file)
{
	mcc_FileBuffer_t *fileBuffer = (mcc_FileBuffer_t *) malloc(sizeof(mcc_FileBuffer_t));
	fileBuffer->filename = file;
	fileBuffer->file = mcc_OpenFile(file, "r");
	fileBuffer->line_no = 1;
	fileBuffer->bufferIndex = 0;
	fileBuffer->chars_read = 0;
    fileBuffer->currentLine.string = NULL;
    fileBuffer->currentLine.length = 0;
    fileBuffer->currentLine.index = 0;
	return fileBuffer;
}

void mcc_DeleteFileBuffer(mcc_FileBuffer_t* buffer)
{
	MCC_ASSERT(buffer->file != NULL);
	fclose(buffer->file);
    if (buffer->currentLine.string != NULL)
    {
        free(buffer->currentLine.string);
    }
	MCC_ASSERT(buffer != NULL);
	free(buffer);
}

static void readFileChunk(mcc_FileBuffer_t *fileBuffer)
{
	fileBuffer->chars_read = fread(fileBuffer->buffer,
								   sizeof(*(fileBuffer->buffer)),
								   FILE_BUFFER_SIZE,
								   fileBuffer->file);
	if (ferror(fileBuffer->file))
	{
		mcc_Error("Unexpected Error (%s) while reading %s\n",
				  strerror(errno), fileBuffer->filename);
	}
	//make life a little easier for ourselves
	fileBuffer->bufferIndex = 0;
}

bool_t mcc_FileBufferEOFReached(mcc_FileBuffer_t *buffer)
{
	return (bool_t) (feof(buffer->file) && buffer->chars_read == 0);
}

mcc_LogicalLine_t *mcc_FileBufferGetNextLogicalLine(mcc_FileBuffer_t *fileBuffer)
{
	mcc_StringBuffer_t *lineBuffer = mcc_CreateStringBuffer();
	bool_t lineIsRead = FALSE;
    if (fileBuffer->currentLine.string != NULL)
    {
        free(fileBuffer->currentLine.string);
    }
	while(!lineIsRead)
	{
		if (fileBuffer->bufferIndex == fileBuffer->chars_read)
		{
			readFileChunk(fileBuffer);
			if (mcc_FileBufferEOFReached(fileBuffer))
			{
				mcc_DeleteStringBuffer(lineBuffer);
				fileBuffer->currentLine.string = NULL;
                fileBuffer->currentLine.index = 0;
                fileBuffer->currentLine.length = 0;
                return &fileBuffer->currentLine;
			}
		}
		while ((fileBuffer->bufferIndex < fileBuffer->chars_read) && !lineIsRead)
		{
			if (fileBuffer->buffer[fileBuffer->bufferIndex] != '\n')
			{
				mcc_StringBufferAppendChar(lineBuffer,
										   fileBuffer->buffer[fileBuffer->bufferIndex++]);
			}
			else
			{
				//Should escaped new lines be able to explicitly handle any of \r or \r\n or \n?
				//Shouldn't the C library return a single '\n' instead of '\r\n' anyway?
				if (fileBuffer->buffer[(fileBuffer->bufferIndex - 1)] == '\\')
				{
					mcc_StringBufferUnappendChar(lineBuffer);
				}
				else
				{
					mcc_StringBufferAppendChar(lineBuffer, '\0');
					lineIsRead = TRUE;
				}
				fileBuffer->bufferIndex++;
				//We want to increase this regardless of whether we have a full logical line or not,
				//so we keep the line numbers in sync with the source file
				fileBuffer->line_no++;
			}
		}
	}
    fileBuffer->currentLine.length = mcc_GetStringBufferLength(lineBuffer);
	fileBuffer->currentLine.string = mcc_DestroyBufferNotString(lineBuffer);
    fileBuffer->currentLine.index = 0;
    return &fileBuffer->currentLine;
}


#if MCC_DEBUG
void printFileBuffer(mcc_FileBuffer_t *buffer)
{
	printf("----- FileBuffer ----- \nfilename:\t%s\nline_no:\t%d\nbufferIndex:\t%d\nchars_read:\t%d\n",
		   buffer->filename, buffer->line_no, buffer->bufferIndex, buffer->chars_read);
	printf("Current Char:\t%d\n", buffer->buffer[buffer->bufferIndex]);
}
#endif
