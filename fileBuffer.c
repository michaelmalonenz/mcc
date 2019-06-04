/**
 Copyright (c) 2012, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "fileBuffer.h"
#include "stringBuffer.h"
#include "mcc.h"

/* A good, meaty base-2 chunk of a file, so we don't start reading the thing
 * from disk a character at a time
 */
#define FILE_BUFFER_SIZE 32768

/* Should avoid allocating these on the stack - it could use up memory much too quickly
 * (stacks are usually limited to around 8MB) Which, I suppose is big enough to hold a 
 * decent amount, but don't I want to leave plenty of room for other stuff?
 */
struct FileBuffer {
   FILE *file;
   const char *filename;
   unsigned short fileNumber;
   unsigned int lineNum;
   unsigned int bufferIndex;
   char buffer[FILE_BUFFER_SIZE + 1];
   size_t charsRead;
   mcc_LogicalLine_t currentLine;
};


#if MCC_DEBUG
void mcc_DebugPrintFileBuffer(mcc_FileBuffer_t *buffer);
#endif

const char *mcc_GetFileBufferFilename(mcc_FileBuffer_t *fileBuffer)
{
   return fileBuffer->filename;
}

unsigned int mcc_GetFileBufferCurrentLineNo(mcc_FileBuffer_t *fileBuffer)
{
   return fileBuffer->lineNum;
}

unsigned short mcc_GetFileBufferFileNumber(mcc_FileBuffer_t *fileBuffer)
{
   return fileBuffer->fileNumber;
}

mcc_FileBuffer_t *mcc_CreateFileBuffer(const char *file)
{
   mcc_FileBuffer_t *fileBuffer = (mcc_FileBuffer_t *) malloc(sizeof(mcc_FileBuffer_t));
   fileBuffer->filename = file;
   fileBuffer->file = mcc_OpenFile(file, "r", &fileBuffer->fileNumber);
   fileBuffer->lineNum = 0;
   fileBuffer->bufferIndex = 0;
   fileBuffer->charsRead = 0;
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
   fileBuffer->charsRead = fread(fileBuffer->buffer,
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
   return (bool_t) (feof(buffer->file) && buffer->charsRead == 0);
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
      if (fileBuffer->bufferIndex == fileBuffer->charsRead)
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
      while ((fileBuffer->bufferIndex < fileBuffer->charsRead) && !lineIsRead)
      {
         if (!isBreakingWhiteSpace(fileBuffer->buffer[fileBuffer->bufferIndex]))
         {
            mcc_StringBufferAppendChar(lineBuffer,
                                       fileBuffer->buffer[fileBuffer->bufferIndex++]);
         }
         else
         {
            if (fileBuffer->buffer[(fileBuffer->bufferIndex - 1)] == '\\')
            {
               mcc_StringBufferUnappendChar(lineBuffer);
            }
            else
            {
               mcc_StringBufferAppendChar(lineBuffer, '\0');
               lineIsRead = TRUE;
            }
            //This might seem weird, but the construction here will allow us to handle any
            //of the 3 different line ending types without handling more than a single line
            //in the buffer.
            if(fileBuffer->buffer[fileBuffer->bufferIndex] == '\r')
            {
               fileBuffer->bufferIndex++;
            }
            if(fileBuffer->buffer[fileBuffer->bufferIndex] == '\n')
            {
               fileBuffer->bufferIndex++;
            }
            //We want to increase this regardless of whether we have a full logical line or not,
            //so we keep the line numbers in sync with the source file
            fileBuffer->lineNum++;
         }
      }
   }
   fileBuffer->currentLine.length = mcc_GetStringBufferLength(lineBuffer);
   fileBuffer->currentLine.string = mcc_DestroyBufferNotString(lineBuffer);
   fileBuffer->currentLine.index = 0;
   return &fileBuffer->currentLine;
}

int SkipWhiteSpace(mcc_LogicalLine_t *line)
{
   int numChars = 0;
   while( (line->index + numChars < line->length) && 
          (isNonBreakingWhiteSpace(line->string[line->index + numChars])) )
   {
      numChars++;
   }
   line->index += numChars;
   return numChars;
}

/**
 * Shift the string left in-place by amountToShift starting at shiftOffset.
 * We don't realloc here, because it's a side effect which would break client
 * referential integrity.
 */
void mcc_ShiftLineLeftAndShrink(mcc_LogicalLine_t *line,
                                uint32_t shiftOffset,
                                int amountToShift)
{
   uint32_t i;
   for (i = 0; i < line->length - (shiftOffset + amountToShift); i++)
   {
      line->string[shiftOffset + i] = line->string[shiftOffset + i + amountToShift];
   }
   line->length -= amountToShift;
   line->string[line->length] = '\0';
}

#if MCC_DEBUG
void mcc_DebugPrintFileBuffer(mcc_FileBuffer_t *buffer)
{
   printf("----- FileBuffer ----- \nfilename:\t%s\nlineNum:\t%u\nbufferIndex:\t%u\ncharsRead:\t%" PRIuPTR "\n",
          buffer->filename, buffer->lineNum, buffer->bufferIndex, (uintptr_t)buffer->charsRead);
   printf("Current Char:\t%d\n", buffer->buffer[buffer->bufferIndex]);
}
#endif
