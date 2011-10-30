#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "tokens.h"

static void mcc_TokeniseLine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handle_string_char_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer,
                                     const char delimiter,
                                     TOKEN_TYPE type);
static void handle_pp_include_filename(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handle_octal_integer_const(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handle_hex_integer_const(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);

static mcc_LogicalLine_t *DealWithComments(mcc_LogicalLine_t* line, mcc_FileBuffer_t *fileBuffer)
{
   unsigned int tempIndex;
   SkipWhiteSpace(line);
   for(tempIndex = line->index; tempIndex < line->length; tempIndex++)
   {
      if (line->string[tempIndex] == '"')
      {
         //need to loop until we find a second " 
      }
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

static void handle_pp_include_filename(mcc_LogicalLine_t *line,
                                       mcc_FileBuffer_t *fileBuffer)
{
   char delimiter;
   TOKEN_TYPE type;
   int filenameLen = 0;
   mcc_Token_t *token = NULL;

   SkipWhiteSpace(line);

   delimiter = line->string[line->index];
   if (delimiter == '"')
   {
      type = TOK_LOCAL_FILE_INC;
   }
   else if (delimiter == '<')
   {
      type = TOK_SYS_FILE_INC;
      delimiter = '>';
   }
   else
   {
      mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                      mcc_GetFileBufferCurrentLineNo(fileBuffer),
                      "Not a recognised character %c for file include\n",
                      delimiter);
   }
   line->index++;

   while (line->string[line->index + filenameLen] != delimiter)
   {
      if (line->index + filenameLen == line->length)
      {
         mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                         mcc_GetFileBufferCurrentLineNo(fileBuffer),
                         "Reached the end of the line while looking for the include file delimiter '%c'\n",
                         delimiter);
      }
      else
      {
         if (line->string[line->index + filenameLen] == '\\' ||
             line->string[line->index + filenameLen] == '\'' ||
             line->string[line->index + filenameLen] == '"')
         {
            mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                            mcc_GetFileBufferCurrentLineNo(fileBuffer),
                            "Invalid character '%c' found in the included file name\n",
                            line->string[line->index + filenameLen]);
         }
         else if (line->string[line->index + filenameLen] == '/')
         {
            if (line->string[line->index + filenameLen + 1] == '/' ||
                line->string[line->index + filenameLen + 1] == '*')
            {
               mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                               mcc_GetFileBufferCurrentLineNo(fileBuffer),
                               "Comments are not allowed inside an include filename\n");
            }
         }
         filenameLen++;
      }
   }

   token =  mcc_CreateToken(&line->string[line->index],
                                         filenameLen, type,
                                         mcc_GetFileBufferCurrentLineNo(fileBuffer));
   token->tokenType = type;
   mcc_AddToken(token);
   line->index += filenameLen + 1; //+1 for the delimiter
   SkipWhiteSpace(line);
   if (line->index != line->length)
   {
      mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                      mcc_GetFileBufferCurrentLineNo(fileBuffer),
                      "Found extra characters after file include\n");
   }
}

static void handle_string_char_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer,
                                     const char delimiter,
                                     TOKEN_TYPE type)
{
   int strLen = 0;
   mcc_Token_t *token = NULL;
   char *type_name;

   if (type == TOK_STR_CONST)
   {
      type_name = "string";
   }
   else if (type == TOK_CHAR_CONST)
   {
      type_name = "character";
   }
   else
   {
      MCC_ASSERT(FALSE);
   }   

   //move the line past the initial delimiter
   line->index++;

   while(line->string[line->index + strLen] != delimiter)
   {
      if ((line->index + strLen) == line->length)
      {
         //The fileBuffer should have already found all the line continuations
         //and we should be able to handle all string constants as a single logical line.
         mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                         mcc_GetFileBufferCurrentLineNo(fileBuffer),
                         "Reached end of line when parsing %s constant and there's no continuation character\n",
                         type_name);
      }
      else if (line->string[line->index + strLen] == '\\')
      {
         mcc_ShiftLineLeftAndShrink(line, line->index + strLen);
         switch (line->string[line->index + strLen])
         {
            case 'a':
               line->string[line->index + strLen] = '\a';
               break;
            case 'b':
               line->string[line->index + strLen] = '\b';
               break;
            case 't':
               line->string[line->index + strLen] = '\t';
               break;
            case 'n':
               line->string[line->index + strLen] = '\n';
               break;
            case 'v':
               line->string[line->index + strLen] = '\v';
               break;
            case 'f':
               line->string[line->index + strLen] = '\f';
               break;
            case 'r':
               line->string[line->index + strLen] = '\r';
               break;
            case '\\':
               line->string[line->index + strLen] = '\\';
               break;
            case '\'':
               line->string[line->index + strLen] = '\'';
               break;
            case '"':
               line->string[line->index + strLen] = '"';
               break;
            case '0':
               handle_octal_integer_const(line, fileBuffer);
               break;
            case 'x':
               handle_hex_integer_const(line, fileBuffer);
               break;
            default:
               mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                               mcc_GetFileBufferCurrentLineNo(fileBuffer),
                               "Unrecognised escape sequence \\%c when parsing %s constant\n",
                               line->string[line->index + strLen], type_name);
               break;
         }
      }
      strLen++;
   }
   token = mcc_CreateToken(&line->string[line->index],
                           strLen, type,
                           mcc_GetFileBufferCurrentLineNo(fileBuffer));
   token->tokenType = type;
   line->index += strLen + 1;
   line = DealWithComments(line, fileBuffer);
   mcc_AddToken(token);
}

static void handle_octal_integer_const(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer))
{
   
}

static void handle_hex_integer_const(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer))
{
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
static void mcc_TokeniseLine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer)
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
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer));
            token->tokenIndex = pp_dir;
            line->index += pp_strlens[pp_dir];
            if (pp_dir == PP_INCLUDE)
            {
               mcc_AddToken(token);
               token = NULL;
               handle_pp_include_filename(line, fileBuffer);
            }
         }
      }
      else if ((current_symbol = mcc_GetSymbol(line)) != SYM_NONE ||
               (toupper(line->string[line->index]) == 'L' &&
                (line->string[line->index+1] == '\'' || 
                 line->string[line->index+1] == '"')))
      {
         if (toupper(line->string[line->index]) == 'L')
         {
            line->index++;
         }
         if (current_symbol == SYM_DOUBLE_QUOTE)
         {
            handle_string_char_const(line, fileBuffer,
                                           '"', TOK_STR_CONST);
         }
         else if (current_symbol == SYM_SINGLE_QUOTE)
         {
            handle_string_char_const(line, fileBuffer,
                                     '\'', TOK_CHAR_CONST);            
         }
         else
         {
            token = mcc_CreateToken(symbols[current_symbol],
                                    symbol_strlens[current_symbol],
                                    TOK_SYMBOL,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer));
            token->tokenIndex = current_symbol;
            line->index += symbol_strlens[current_symbol];
            //addressof and logical and need to be differentiated by the parser
            //based on context _and_ corrected
         }
      }
      else if ((current_operator = mcc_GetOperator(line)) != OP_NONE)
      {
         token = mcc_CreateToken(operators[current_operator],
                                 operator_strlens[current_operator], 
                                 TOK_OPERATOR,
                                 mcc_GetFileBufferCurrentLineNo(fileBuffer));
         token->tokenIndex = current_operator;
         line->index += operator_strlens[current_operator];
      }
      else if (isWordChar(line->string[line->index]))
      {
         MCC_KEYWORD keyword = mcc_GetKeyword(line);
         if (keyword != KEY_NONE)
         {
            token = mcc_CreateToken(keywords[keyword], keyword_strlens[keyword],
                                    TOK_KEYWORD,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer));
            token->tokenIndex = keyword;
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
                                    TOK_IDENTIFIER,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer));
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
                                 TOK_NUMBER,
                                 mcc_GetFileBufferCurrentLineNo(fileBuffer));
         line->index += numLen;
      }
      else
      {
         mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                         mcc_GetFileBufferCurrentLineNo(fileBuffer),
                         "Not a recognised character: '%c'.  This is probably a bug in the tokeniser.\n",
                         line->string[line->index]);
      }
     
      if (token != NULL)
      {
         mcc_AddToken(token);
         token = NULL;
      }
      SkipWhiteSpace(line);
   }
}

