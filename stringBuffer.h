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
#ifndef MCC_STRING_BUFFER_H
#define MCC_STRING_BUFFER_H

#include <stdlib.h>
#include "config.h"

typedef struct StringBuffer mcc_StringBuffer_t;

mcc_StringBuffer_t *mcc_CreateStringBuffer(void);

void mcc_DeleteStringBuffer(mcc_StringBuffer_t *buffer);

char *mcc_DestroyBufferNotString(mcc_StringBuffer_t *buffer);

void mcc_StringBufferAppendChar(mcc_StringBuffer_t *buffer, const char c);

void mcc_StringBufferUnappendChar(mcc_StringBuffer_t *buffer);

unsigned long mcc_GetStringBufferLength(mcc_StringBuffer_t *buffer);

const char *mcc_StringBufferGetString(mcc_StringBuffer_t *buffer);

int mcc_StringBufferStrncmp(mcc_StringBuffer_t *buffer, const char *string, size_t length);

#if MCC_DEBUG
void mcc_PrintStringBuffer(mcc_StringBuffer_t *buffer);
unsigned long mcc_StringBufferGetBufferSize(mcc_StringBuffer_t *buffer);
#endif /* MCC_DEBUG */

#endif /* MCC_STRING_BUFFER_H */
