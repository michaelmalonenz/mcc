#ifndef MCC_FILE_BUFFER_H
#define MCC_FILE_BUFFER_H

#include <stdio.h>
#include "mcc.h"

typedef struct FileBuffer mcc_FileBuffer_t;

typedef struct LogicalLine {
    unsigned char *string;
    unsigned int index;
    unsigned int length;
} mcc_LogicalLine_t;

mcc_FileBuffer_t *mcc_CreateFileBuffer(const char *file);

void mcc_DeleteFileBuffer(mcc_FileBuffer_t* buffer);

bool_t mcc_FileBufferEOFReached(mcc_FileBuffer_t *buffer);

mcc_LogicalLine_t *mcc_FileBufferGetNextLogicalLine(mcc_FileBuffer_t *fileBuffer);


#endif /* MCC_FILE_BUFFER_H */
