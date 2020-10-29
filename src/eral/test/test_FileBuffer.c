/**
 Copyright (c) 2011, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdio.h>
#include <string.h>

#include "mcc.h"
#include "liberal.h"
#include "fileBuffer.h"

#define FILE_CONTENTS "\
This is the first line\n\
This is the second line\n\
The third logical line is spread over two physical lines\\\n\
 see - this is the rest of the third logical line\n\
this is the \"fourth\" line and it has whitespace     \\\n\
before the continuation character\n\
The Fifth logical line has a dos line ending\r\n\
No Line Ending at EOF"

enum lines { FIRST_LINE, SECOND_LINE, THIRD_LINE, FOURTH_LINE, FIFTH_LINE, SIXTH_LINE, NUM_LINES };
const char *lines[NUM_LINES] = { [FIRST_LINE] = "This is the first line",
                                 [SECOND_LINE] = "This is the second line",
                                 [THIRD_LINE] = "The third logical line is spread over two physical lines see - this is the rest of the third logical line",
                                 [FOURTH_LINE] = "this is the \"fourth\" line and it has whitespace     before the continuation character",
                                 [FIFTH_LINE] = "The Fifth logical line has a dos line ending",
                                 [SIXTH_LINE] = "No Line Ending at EOF"};

#define FILENAME "test_fileBufferHeader.h"

static void SetupFileBufferTest(void)
{
   FILE *file = fopen(FILENAME, "w+");
   fwrite(FILE_CONTENTS, sizeof(char), strlen(FILE_CONTENTS), file);
   fclose(file);
   mcc_FileOpenerInitialise();
}

static void TearDownFileBufferTest(void)
{
   remove(FILENAME);
   mcc_FileOpenerDelete();
}


static void test_fileBuffer(void)
{
   eral_FileBuffer_t *testBuffer = eral_CreateFileBuffer(FILENAME);
   eral_LogicalLine_t *line = NULL;
   int i = 0;
   while(!eral_FileBufferEOFReached(testBuffer))
   {
      line = eral_FileBufferGetNextLogicalLine(testBuffer);
      printf("Test Line  : '%s'\n", lines[i]);
      printf("Value line : '%s'\n", line->string);
      MCC_ASSERT(strncmp(lines[i], line->string, line->length) == 0);
      i++;
   }
   MCC_ASSERT(i == NUM_LINES);
   eral_DeleteFileBuffer(testBuffer);
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   SetupFileBufferTest();
   test_fileBuffer();
   TearDownFileBufferTest();
   return 0;
}
