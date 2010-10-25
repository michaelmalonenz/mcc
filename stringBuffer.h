#ifndef MCC_STRING_BUFFER_H
#define MCC_STRING_BUFFER_H

typedef struct StringBuffer mcc_StringBuffer_t;

mcc_StringBuffer_t *mcc_CreateStringBuffer(void);

void mcc_DeleteStringBuffer(mcc_StringBuffer_t *buffer);

void mcc_AppendChar(mcc_StringBuffer_t *buffer, const signed char c);
#endif
