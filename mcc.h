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

void mcc_PreprocessFile(const char *in_filename, const char *out_filename);
void mcc_ParseFile(const char *filename);

typedef _Bool bool_t;

#ifndef TRUE
#define TRUE ((bool_t) 1)
#endif

#ifndef FALSE
#define FALSE ((bool_t) 0)
#endif

#endif /* MCC_H */
