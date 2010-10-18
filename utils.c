#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void mcc_Error(const char *format, ...)
{
	fprintf(stderr, format);
}
