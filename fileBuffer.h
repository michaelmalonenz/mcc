/**
    mcc a lightweight compiler for developers, not machines
    Copyright (C) 2011 Michael Malone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

inline int SkipWhiteSpace(mcc_LogicalLine_t *line);
void mcc_ShiftLineLeftAndShrink(mcc_LogicalLine_t *line, uint32_t shiftOffset, int amountToShift);

const char *mcc_GetFileBufferFilename(mcc_FileBuffer_t *fileBuffer);
unsigned short mcc_GetFileBufferFileNumber(mcc_FileBuffer_t *fileBuffer);
unsigned int mcc_GetFileBufferCurrentLineNo(mcc_FileBuffer_t *fileBuffer);
#endif /* MCC_FILE_BUFFER_H */
