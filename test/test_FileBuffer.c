#include <stdio.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"

#define FILE_CONTENTS "\
This is the first line\
This is the second line\
The third logical line is spread over two phyical lines\\\
see - this is the rest of the third logical line\
this is the \"fourth\" line and it has whitespace     \\\
before the continuation character\
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
