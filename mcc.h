#include <stdarg.h>
#include <stdio.h>

#define mcc_Error(...) fprintf (stderr, __VA_ARGS__)

void mcc_PreprocessFile(const char *in_filename, const char *out_filename);
void mcc_ParseFile(const char *filename);
