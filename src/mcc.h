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
#ifndef MCC_H
#define MCC_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>

#define MCC_MAX_INPUT_FILES USHRT_MAX

#define mcc_Error(...) fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE)

#define mcc_PrettyError(file, lineno, col, ...) fprintf(stderr, "%s:%d:%u ", file, lineno, col); \
   mcc_Error(__VA_ARGS__)

#define mcc_PrettyErrorToken(token, ...) \
   mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),\
      token->lineno,\
      token->line_index+1,\
      __VA_ARGS__)

#define mcc_Warning(token, format, ...) \
   fprintf(stderr, "%s:%d:%d Warning: ",\
      mcc_ResolveFileNameFromNumber(token->fileno),\
      token->lineno,\
      token->line_index+1);\
   fprintf(stderr, format, __VA_ARGS__);

#if MCC_DEBUG
# define MCC_ASSERT(x) assert(x)
#else
# define MCC_ASSERT(x)
#endif

#ifndef UNUSED
#define UNUSED(x) x __attribute__ ((unused))
#endif

/* If the two are equal, just return one or the other */
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )
#define min(a, b) ( ((a) < (b)) ? (a) : (b) )


#define isWhiteSpace(c) ((c) == '\t' || (c) == '\v' || \
                         (c) == '\r' || (c) == '\n' || \
                         (c) == '\v' || (c) == ' ')

#define isNonBreakingWhiteSpace(c) ((c) == ' ' || (c) == '\t' || (c) == '\v')

#define isBreakingWhiteSpace(c) ((c) == '\r' || (c) == '\n')

#define isWordChar(c) ( ((c) >= 'A' && (c) <= 'Z') || \
                        ((c) >= 'a' && (c) <= 'z') || \
                        ((c) >= '0' && (c) <= '9') || \
                        ((c) == '_') )

#define isNumber(d) ( ((d) >= '0') && ((d) <= '9') )

#define isOctalChar(d) ( ((d) >= '0') && ((d) <= '7') )

#define isHexChar(x) ( isNumber(x) || (toupper(x) >= 'A' && toupper(x) <= 'F') )

FILE *mcc_OpenFile(const char *filename, const char *flags, unsigned short *out_fileno);
void mcc_FileOpenerInitialise(void);
void mcc_FileOpenerDelete(void);
const char *mcc_ResolveFileNameFromNumber(const unsigned short fileno);

/**
 * These functions allocate dynamic memory for the returned filename.
 * It is the caller's responsibility to free this memory.
 */
char *mcc_FindLocalInclude(const char *filename);
char *mcc_FindSystemInclude(const char *filename);
void mcc_FileOpenerLocalIncAppendDir(const char *dir);

#endif /* MCC_H */
