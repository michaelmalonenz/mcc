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

FILE *mcc_OpenFile(const char *filename, char *flags, unsigned short *out_fileno);
const char *mcc_ResolveFileNameFromNumber(const unsigned short fileno);

const char *mcc_FindLocalInclude(const char *filename);
const char *mcc_FindSystemInclude(const char *filename);

void mcc_PreprocessCurrentTokens(void);
void mcc_TokeniseFile(const char *inFilename);

#endif /* MCC_H */
