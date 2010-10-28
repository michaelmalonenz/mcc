#ifndef MCC_H
#define MCC_H

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include "config.h"

#define MCC_MAX_INPUT_FILES USHRT_MAX

#define mcc_Error(...) fprintf (stderr, __VA_ARGS__)

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

void mcc_PreprocessFile(FILE *inFile, FILE *outFile);
void mcc_TokeniseString(const char *string, const unsigned int length);

#endif /* MCC_H */
