#ifndef MCC_STRING_BUFFER_H
#define MCC_STRING_BUFFER_H

#include "config.h"

typedef struct StringBuffer mcc_StringBuffer_t;

mcc_StringBuffer_t *mcc_CreateStringBuffer(void);

void mcc_DeleteStringBuffer(mcc_StringBuffer_t *buffer);

void mcc_StringBufferAppendChar(mcc_StringBuffer_t *buffer, const unsigned char c);

int mcc_StringBufferGetStringLength(mcc_StringBuffer_t *buffer);

#if MCC_DEBUG
unsigned int mcc_StringBufferGetBufferSize(mcc_StringBuffer_t *buffer);
#endif /* MCC_DEBUG */

#endif /* MCC_STRING_BUFFER_H */
