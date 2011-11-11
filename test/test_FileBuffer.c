/**
    mcc a lightweight compiler for developers, not machines
    Copyright (C) 2011 Michael Malone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#include <stdio.h>
#include <string.h>

#define MCC_DEBUG 1
#include "mcc.h"
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
}

static void TearDownFileBufferTest(void)
{
   remove(FILENAME);
}


static void test_fileBuffer(void)
{
   mcc_FileBuffer_t *testBuffer = mcc_CreateFileBuffer(FILENAME);
   mcc_LogicalLine_t *line = NULL;
   int i = 0;
   while(!mcc_FileBufferEOFReached(testBuffer))
   {
      line = mcc_FileBufferGetNextLogicalLine(testBuffer);
      printf("Test Line  : '%s'\n", lines[i]);
      printf("Value line : '%s'\n", line->string);
      MCC_ASSERT(strncmp(lines[i], line->string, line->length) == 0);
      i++;
   }
   MCC_ASSERT(i == NUM_LINES);
   mcc_DeleteFileBuffer(testBuffer);
}

int main(int UNUSED(argc), char UNUSED(**argv))
{
   SetupFileBufferTest();
   test_fileBuffer();
   TearDownFileBufferTest();
   return 0;
}
