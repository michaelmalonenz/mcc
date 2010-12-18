#include <stdio.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"

#define FILE_CONTENTS "\
#ifndef MCC_H \
#define MCC_H \
\
#include <stdarg.h> \
#include <stdio.h> \
#include <limits.h> \
#include <assert.h> \
\
#include \"config.h\" \
\
#define MCC_MAX_INPUT_FILES USHRT_MAX \
\
#define mcc_PrettyError(file, lineno, ...) fprintf(\"%s:%d\n\", file, lineno); \\ \
   fprintf(stderr, __VA_ARGS__) \
\
#define mcc_Error(...) fprintf(stderr, __VA_ARGS__); exit(1) \
\
#if MCC_DEBUG \
#define MCC_ASSERT(x) assert(x) \
#else \
#define MCC_ASSERT(x) \
#endif \
\
typedef _Bool bool_t; \
\
#ifndef TRUE \
#define TRUE ((bool_t) 1) \
#endif \
\
#ifndef FALSE \
#define FALSE ((bool_t) 0) \
#endif \
\
#ifndef UNUSED \
#define UNUSED(x) x __attribute__ ((unused)) \
#endif \
\
/* If the two are equal, just return one or the other */ \
#define max(a, b) ( ((a) > (b)) ? (a) : (b) ) \
#define min(a, b) ( ((a) < (b)) ? (a) : (b) ) \
\
\
#define isWhiteSpace(c) ((c) == '\t' || (c) == '\v' ||	\\ \
                         (c) == '\r' || (c) == '\n' ||  \\ \
                         (c) == '\v' || (c) == ' ') \
\
#define isNonBreakingWhiteSpace(c) ((c) == ' ' || (c) == '\t' || (c) == '\v') \
\
#define isWordChar(c) ( ((c) >= 'A' && (c) <= 'Z') || \\ \
                        ((c) >= 'a' && (c) <= 'z') || \\ \
                        ((c) >= '0' && (c) <= '9') || \\ \
                        ((c) == '_') ) \
\
#define isNumber(d) ( ((d) >= '0') && ((d) <= '9') ) \
\
FILE *mcc_OpenFile(const char *filename, char *flags); \
\
const char *mcc_FindLocalInclude(const char *filename); \
const char *mcc_FindSystemInclude(const char *filename); \
\
void mcc_PreprocessFile(const char *inFilename, FILE *outFile); \
void mcc_TokeniseString(const char *string, const unsigned int length); \
\
#endif /* MCC_H */ \
"

#define FILENAME "test_fileBufferHeader.h"

static void SetupFileBufferTest(void)
{
   FILE *file = fopen(FILENAME, "w+");
   fwrite(FILE_CONTENTS, sizeof(char), strlen(FILE_CONTENTS), file);
   fclose(file);
}

static void TearDownFileBufferTest(void)
{
   remove(FILENAME);
}


static void test_fileBuffer(void)
{
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   SetupFileBufferTest();
   test_fileBuffer();
   TearDownFileBufferTest();
   return 0;
}
