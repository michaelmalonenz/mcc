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
#ifndef MCC_FILE_BUFFER_H
#define MCC_FILE_BUFFER_H

#include <stdio.h>
#include <stdint.h>
#include "mcc.h"

typedef struct FileBuffer mcc_FileBuffer_t;

typedef struct LogicalLine {
   char *string;
   unsigned int index;
   unsigned int length;
} mcc_LogicalLine_t;

mcc_FileBuffer_t *mcc_CreateFileBuffer(const char *file);

void mcc_DeleteFileBuffer(mcc_FileBuffer_t* buffer);

bool_t mcc_FileBufferEOFReached(mcc_FileBuffer_t *buffer);

mcc_LogicalLine_t *mcc_FileBufferGetNextLogicalLine(mcc_FileBuffer_t *fileBuffer);

int SkipWhiteSpace(mcc_LogicalLine_t *line);
void mcc_ShiftLineLeftAndShrink(mcc_LogicalLine_t *line, uint32_t shiftOffset, int amountToShift);

const char *mcc_GetFileBufferFilename(mcc_FileBuffer_t *fileBuffer);
unsigned short mcc_GetFileBufferFileNumber(mcc_FileBuffer_t *fileBuffer);
unsigned int mcc_GetFileBufferCurrentLineNo(mcc_FileBuffer_t *fileBuffer);

#if MCC_DEBUG
void mcc_DebugPrintFileBuffer(mcc_FileBuffer_t *buffer);
#endif

#endif /* MCC_FILE_BUFFER_H */