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
#ifndef ERAL_STRING_BUFFER_H
#define ERAL_STRING_BUFFER_H

#include <stdlib.h>

typedef struct StringBuffer eral_StringBuffer_t;

eral_StringBuffer_t *eral_CreateStringBuffer(void);

void eral_DeleteStringBuffer(eral_StringBuffer_t *buffer);

char *eral_DestroyBufferNotString(eral_StringBuffer_t *buffer);

void eral_StringBufferAppendChar(eral_StringBuffer_t *buffer, const char c);

void eral_StringBufferAppendString(eral_StringBuffer_t *buffer, const char *string);

void eral_StringBufferUnappendChar(eral_StringBuffer_t *buffer);

unsigned long eral_GetStringBufferLength(eral_StringBuffer_t *buffer);

const char *eral_StringBufferGetString(eral_StringBuffer_t *buffer);

int eral_StringBufferStrncmp(eral_StringBuffer_t *buffer, const char *string, size_t length);

#if MCC_DEBUG
void eral_PrintStringBuffer(eral_StringBuffer_t *buffer);
unsigned long eral_StringBufferGetBufferSize(eral_StringBuffer_t *buffer);
#endif /* MCC_DEBUG */

#endif /* ERAL_STRING_BUFFER_H */
