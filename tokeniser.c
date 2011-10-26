#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "tokens.h"

static void mcc_TokeniseLine(mcc_LogicalLine_t *line, const int lineno);

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
         mcc_TokeniseLine(logicalLine, mcc_GetFileBufferCurrentLineNo(fileBuffer));
      }
   }
   mcc_DeleteFileBuffer(fileBuffer);
}

//by providing a logical line, we are guaranteed to only have whole tokens.
static void mcc_TokeniseLine(mcc_LogicalLine_t *line, const int current_lineno)
{
   MCC_OPERATOR current_operator = OP_NONE;
   MCC_SYMBOL current_symbol = SYM_NONE;
   mcc_Token_t *token = NULL;

   SkipWhiteSpace(line);
   while(line->index < line->length)
   {
      if (line->string[line->index] == '#')
      {
         line->index++;
         if (line->string[line->index] == '#')
         {
            //it's a preprocessor join operator
         }
         else
         {
            PREPROC_DIRECTIVE pp_dir = mcc_GetPreprocessorDirective(line);
            MCC_ASSERT(pp_dir != PP_NONE);
            token = mcc_CreateToken(preprocessor_directives[pp_dir], 
                                    pp_strlens[pp_dir], TOK_PP_DIRECTIVE,
                                    current_lineno);
            line->index += pp_strlens[pp_dir];
         }
      }
      else if (isWordChar(line->string[line->index]))
      {
         MCC_KEYWORD keyword = mcc_GetKeyword(line);
         if (keyword != KEY_NONE)
         {
            token = mcc_CreateToken(keywords[keyword], keyword_strlens[keyword],
                                    TOK_KEYWORD, current_lineno);
            line->index += keyword_strlens[keyword];
         }
         else //it's an identifier
         {
            int identLen = 1;
            while ((isWordChar(line->string[line->index + identLen]) ||
                    isNumber(line->string[line->index + identLen])) &&
                   ((line->index + identLen) < line->length))
            {
               identLen++;
            }
            token = mcc_CreateToken(&line->string[line->index], identLen,
                                    TOK_IDENTIFIER, current_lineno);
            line->index += identLen;
         }
      }
      else if (isNumber(line->string[line->index]))
      {
         int numLen = 1;
         while (isNumericChar(line->string[line->index + numLen]) &&
                 line->index + numLen < line->length)
         {
            numLen++;
         }
         token = mcc_CreateToken(&line->string[line->index], numLen,
                                 TOK_NUMBER, current_lineno);
         line->index += numLen;
      }
      else if ((current_symbol = mcc_GetSymbol(line)) != SYM_NONE)
      {
         token = mcc_CreateToken(symbols[current_symbol],
                                 symbol_strlens[current_symbol],
                                 TOK_SYMBOL, current_lineno);
         line->index += symbol_strlens[current_symbol];
         //how does one tell the difference between a logical and and addressof
      }
      else if ((current_operator = mcc_GetOperator(line)) != OP_NONE)
      {
         token = mcc_CreateToken(operators[current_operator],
                                 operator_strlens[current_operator],
                                 TOK_OPERATOR, current_lineno);
         line->index += operator_strlens[current_operator];
      }
      else
      {
         mcc_Error("Not a recognised character: '%c'\n", line->string[line->index]);
      }
      if (token != NULL)
      {
         mcc_AddToken(token);
         token = NULL;
      }
      SkipWhiteSpace(line);
   }
}

