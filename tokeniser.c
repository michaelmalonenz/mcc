#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "mcc.h"
#include "fileBuffer.h"

#include "stringBuffer.h"
static void mcc_TokeniseLine(mcc_LogicalLine_t *line, mcc_FileBuffer_t UNUSED(*fileBuffer));

static mcc_LogicalLine_t *DealWithComments(mcc_LogicalLine_t* line, mcc_FileBuffer_t *fileBuffer)
{
   unsigned int tempIndex;
   SkipWhiteSpace(line);
   for(tempIndex = line->index; tempIndex < line->length; tempIndex++)
   {
      if (line->string[tempIndex] == '/')
      {
         if (line->string[tempIndex+1] == '/')
         {
            while(tempIndex < line->length)
            {
               line->string[tempIndex++] = ' ';
            }
         }
         else if (line->string[tempIndex+1] == '*')
         {
            line->string[tempIndex] = ' ';
            line->string[tempIndex+1] = ' ';
            while(!mcc_FileBufferEOFReached(fileBuffer))
            {
               while(tempIndex < line->length)
               {
                  if(line->string[tempIndex] == '*' &&
                     line->string[tempIndex+1] == '/')
                  {
                     line->string[tempIndex++] = ' ';
                     line->string[tempIndex] = ' ';
                     SkipWhiteSpace(line);
                     return line;
                  }
                  line->string[tempIndex++] = ' ';
               }
               tempIndex = 0;
               line = mcc_FileBufferGetNextLogicalLine(fileBuffer);
            }
         }
      }
   }
   SkipWhiteSpace(line);
   return line;
}

void mcc_TokeniseFile(const char *inFilename)
{
   mcc_FileBuffer_t *fileBuffer = mcc_CreateFileBuffer(inFilename);
   mcc_LogicalLine_t *logicalLine = NULL;

   while(!mcc_FileBufferEOFReached(fileBuffer))
   {
      logicalLine = mcc_FileBufferGetNextLogicalLine(fileBuffer);
      if (logicalLine->length > 0)
      {
         logicalLine = DealWithComments(logicalLine, fileBuffer);
         if (logicalLine->index == logicalLine->length)
         {
            continue;
         }
         mcc_TokeniseLine(logicalLine, fileBuffer);
      }
   }
   mcc_DeleteFileBuffer(fileBuffer);
}

//by providing a logical line, we are guaranteed to only have whole tokens.
static void mcc_TokeniseLine(mcc_LogicalLine_t *line, mcc_FileBuffer_t UNUSED(*fileBuffer))
{
   MCC_OPERATOR current_operator = OP_NONE;
   MCC_SYMBOL current_symbol = SYM_NONE;

   SkipWhiteSpace(line);
   if (line->string[line->index] == '#')
   {
      //it's a pre-processor directive
   }
   else if (isWordChar(line->string[line->index]))
   {
      //either a reserved word or identifier
   }
   else if (isNumericChar(line->string[line->index]))
   {
      //it's a number
   }
   else if ((current_symbol = mcc_GetSymbol(line)) != SYM_NONE)
   {
      //it's a symbol
   }
   else if ((current_operator = mcc_GetOperator(line)) != OP_NONE)
   {
      //it's an operator
   }
   else
   {
      //it's an error
   }
}

