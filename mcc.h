#ifndef MCC_H
#define MCC_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "config.h"

#define MCC_MAX_INPUT_FILES USHRT_MAX

#define mcc_PrettyError(file, lineno, ...) fprintf("%s:%d\n", file, lineno); \
   fprintf(stderr, __VA_ARGS__)

#define mcc_Error(...) fprintf(stderr, __VA_ARGS__); exit(1)

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

#define isNumericChar(d) ( (((d) >= '0') && ((d) <= '9')) || \
                           (d) == 'e' || (d) == 'E' || (d) == '.')

FILE *mcc_OpenFile(const char *filename, char *flags);

const char *mcc_FindLocalInclude(const char *filename);
const char *mcc_FindSystemInclude(const char *filename);

void mcc_PreprocessFile(const char *inFilename, FILE *outFile);
void mcc_TokeniseFile(const char *inFilename);

#endif /* MCC_H */
