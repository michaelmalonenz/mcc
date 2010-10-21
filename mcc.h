#ifndef MCC_H
#define MCC_H

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>

#define MCC_MAX_INPUT_FILES USHRT_MAX

#define mcc_Error(...) fprintf (stderr, __VA_ARGS__)

void mcc_PreprocessFile(const char *in_filename, const char *out_filename);
void mcc_ParseFile(const char *filename);


#endif /* MCC_H */

