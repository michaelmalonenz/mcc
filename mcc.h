#include <stdarg.h>

#define mcc_Error(...) fprintf (stderr, __VA_ARGS__)

void mcc_ParseFile(const char *filename);
