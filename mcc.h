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
#ifndef MCC_H
#define MCC_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>

#include "config.h"

#define MCC_MAX_INPUT_FILES USHRT_MAX

#define mcc_Error(...) fprintf(stderr, __VA_ARGS__); exit(1)

#define mcc_PrettyError(file, lineno, ...) fprintf(stderr, "%s:%d ", file, lineno); \
   mcc_Error(__VA_ARGS__)


#if MCC_DEBUG
#define MCC_ASSERT(x) assert(x)
#else
#define MCC_ASSERT(x)
#endif

typedef _Bool bool_t;

#ifndef TRUE
#define TRUE ((bool_t) 1)
#endif

#ifndef FALSE
#define FALSE ((bool_t) 0)
#endif

#ifndef UNUSED
#define UNUSED(x) x __attribute__ ((unused))
#endif

/* If the two are equal, just return one or the other */
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )
#define min(a, b) ( ((a) < (b)) ? (a) : (b) )


#define isWhiteSpace(c) ((c) == '\t' || (c) == '\v' ||	\
                         (c) == '\r' || (c) == '\n' ||  \
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

#define isNumericChar(d) ( (((d) >= '0') && ((d) <= '9')) || \
                           (d) == 'e' || (d) == 'E' || (d) == '.')

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

#endif /* MCC_H */
