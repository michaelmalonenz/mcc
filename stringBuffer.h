#ifndef MCC_STRING_BUFFER_H
#define MCC_STRING_BUFFER_H

#include <stdlib.h>
#include "config.h"

typedef struct StringBuffer mcc_StringBuffer_t;

mcc_StringBuffer_t *mcc_CreateStringBuffer(void);

void mcc_DeleteStringBuffer(mcc_StringBuffer_t *buffer);

unsigned char *mcc_DestroyBufferNotString(mcc_StringBuffer_t *buffer);

void mcc_StringBufferAppendChar(mcc_StringBuffer_t *buffer, const unsigned char c);

void mcc_StringBufferUnappendChar(mcc_StringBuffer_t *buffer);

unsigned int mcc_GetStringBufferLength(mcc_StringBuffer_t *buffer);

int mcc_StringBufferStrncmp(mcc_StringBuffer_t *buffer, const char *string, size_t length);

#if MCC_DEBUG
void mcc_PrintStringBuffer(mcc_StringBuffer_t *buffer);
unsigned int mcc_StringBufferGetBufferSize(mcc_StringBuffer_t *buffer);
#endif /* MCC_DEBUG */

#endif /* MCC_STRING_BUFFER_H */
