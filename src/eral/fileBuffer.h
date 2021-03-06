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
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#ifndef ERAL_FILE_BUFFER_H
#define ERAL_FILE_BUFFER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "liberal.h"

typedef struct FileBuffer eral_FileBuffer_t;

typedef struct LogicalLine {
   char *string;
   unsigned int index;
   unsigned int length;
} eral_LogicalLine_t;

eral_FileBuffer_t *eral_CreateFileBuffer(const char *file);

void eral_DeleteFileBuffer(eral_FileBuffer_t* buffer);

bool eral_FileBufferEOFReached(eral_FileBuffer_t *buffer);

eral_LogicalLine_t *eral_FileBufferGetNextLogicalLine(eral_FileBuffer_t *fileBuffer);

void eral_ShiftLineLeftAndShrink(eral_LogicalLine_t *line, uint32_t shiftOffset, int amountToShift);

const char *eral_GetFileBufferFilename(eral_FileBuffer_t *fileBuffer);
unsigned short eral_GetFileBufferFileNumber(eral_FileBuffer_t *fileBuffer);
unsigned int eral_GetFileBufferCurrentLineNo(eral_FileBuffer_t *fileBuffer);

#if ERAL_DEBUG
void eral_DebugPrintFileBuffer(eral_FileBuffer_t *buffer);
#endif

#endif /* ERAL_FILE_BUFFER_H */
