/**
 Copyright (c) 2012, Michael Malone
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "config.h"
#include "mcc.h"
#include "liberal.h"
#include "tokens.h"
#include "tokenList.h"


static void mcc_TokeniseLine(mcc_LogicalLine_t *line,
                             mcc_FileBuffer_t *fileBuffer,
                             mcc_TokenListIterator_t *iter);

static void handle_string_char_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer,
                                     const char delimiter,
                                     TOKEN_TYPE type,
                                     mcc_TokenListIterator_t *iter);
static void handle_pp_include_filename(mcc_LogicalLine_t *line,
                                       mcc_FileBuffer_t *fileBuffer,
                                       mcc_TokenListIterator_t *iter);
static char handle_octal_integer_const(mcc_LogicalLine_t *line,
                                       mcc_FileBuffer_t *fileBuffer);
static char handle_hex_integer_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer);
static void handle_whitespace(mcc_LogicalLine_t *line,
                              mcc_FileBuffer_t *fileBuffer,
                              mcc_TokenListIterator_t *iter);

static void handle_whitespace(mcc_LogicalLine_t *line,
                              mcc_FileBuffer_t *fileBuffer,
                              mcc_TokenListIterator_t *iter)
{
   if (SkipWhiteSpace(line) > 0)
   {
      mcc_CreateAndAddWhitespaceToken(line->index+1,
                                      mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                      mcc_GetFileBufferFileNumber(fileBuffer),
                                      iter);
   }
}

static void handle_pp_include_filename(mcc_LogicalLine_t *line,
                                       mcc_FileBuffer_t *fileBuffer,
                                       mcc_TokenListIterator_t *iter)
{
   char delimiter;
   TOKEN_TYPE type = TOK_NONE;
   int filenameLen = 0;
   mcc_Token_t *token = NULL;

   handle_whitespace(line, fileBuffer, iter);

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
                      line->index,
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
                         line->index,
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
                            line->index,
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
                               line->index,
                               "Comments are not allowed inside an include filename\n");
            }
         }
         filenameLen++;
      }
   }

   token =  mcc_CreateToken(&line->string[line->index],
                            filenameLen, type,
                            TOK_UNSET_INDEX,
                            line->index+1,
                            mcc_GetFileBufferCurrentLineNo(fileBuffer),
                            mcc_GetFileBufferFileNumber(fileBuffer));
   mcc_InsertToken(token, iter);
   line->index += filenameLen + 1; //+1 for the delimiter
   handle_whitespace(line, fileBuffer, iter);
}

static void handle_string_char_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer,
                                     const char delimiter,
                                     TOKEN_TYPE type,
                                     mcc_TokenListIterator_t *iter)
{
   int strLen = 0;
   mcc_Token_t *token = NULL;
   const char *type_name;

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
                         line->index,
                         "Reached end of line when parsing %s constant and there's no continuation character\n",
                         type_name);
      }
      else if (line->string[line->index + strLen] == '\\')
      {
         mcc_ShiftLineLeftAndShrink(line, line->index + strLen, 1);
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
               line->string[line->index + strLen] = handle_octal_integer_const(line, fileBuffer);
               break;
            case 'x':
               line->string[line->index + strLen] = handle_hex_integer_const(line, fileBuffer);
               break;
            default:
               mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                               mcc_GetFileBufferCurrentLineNo(fileBuffer),
                               line->index,
                               "Unrecognised escape sequence \\%c when parsing %s constant\n",
                               line->string[line->index + strLen], type_name);
               break;
         }
      }
      strLen++;
   }
   token = mcc_CreateToken(&line->string[line->index],
                           strLen, type,
                           TOK_UNSET_INDEX,
                           line->index+1,
                           mcc_GetFileBufferCurrentLineNo(fileBuffer),
                           mcc_GetFileBufferFileNumber(fileBuffer));
   line->index += strLen + 1;
   mcc_InsertToken(token, iter);
}

static char handle_octal_integer_const(mcc_LogicalLine_t *line,
                                       mcc_FileBuffer_t *fileBuffer)
{
   int intLen = 0, j;
   long long number = 0;
   unsigned long long mask = 0xFF;

   while (isOctalChar(line->string[line->index + intLen]))
   {
      intLen++;
   }   
   for (j = 0; j < intLen; j++)
   {
      number += (line->string[line->index + j] - '0') * (long long)pow(8, intLen-j-1);
   }
   if (number != (long long)(number & mask))
   {
      mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                      mcc_GetFileBufferCurrentLineNo(fileBuffer),
                      line->index,
                      "Octal integer constant '%lld' exceeds allowed precision\n",
                      number);
   }
   mcc_ShiftLineLeftAndShrink(line, line->index, intLen-1);
   return (char) number;
}

static char convertHexCharToNum(char character)
{
   character = (char) toupper(character);
   if (character >= '0' && character <= '9')
   {
      return character - '0';
   }
   else if (character >= 'A' && character <= 'F')
   {
      return (character - 'A') + 10;
   }
   return 0;
}

static char handle_hex_integer_const(mcc_LogicalLine_t *line,
                                     mcc_FileBuffer_t *fileBuffer)
{
   int intLen = 1, j;
   long long number = 0;

   while (isHexChar(line->string[line->index + intLen]))
   {
      intLen++;
   }

   for (j = 1; j < intLen; j++)
   {
      number |= convertHexCharToNum(line->string[line->index + j]) << ((intLen - j - 1) * 4);
   }

   if ((long long)(number & 0xFF) != number)
   {
      char *tempStr = (char *) malloc((intLen + 1) * sizeof(char));
      memcpy(tempStr, &line->string[line->index + 1], intLen-1);
      tempStr[intLen-1] = 0;
      mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                      mcc_GetFileBufferCurrentLineNo(fileBuffer),
                      line->index,
                      "hexadecimal integer constant \\x%s exceeds allowed precision\n",
                      tempStr);
   }

   mcc_ShiftLineLeftAndShrink(line, line->index, intLen-2);
   return (char) number;
}
                                       
void mcc_TokeniseFile(const char *inFilename, 
                      mcc_TokenListIterator_t *iter)
{
   mcc_FileBuffer_t *fileBuffer = mcc_CreateFileBuffer(inFilename);
   mcc_LogicalLine_t *logicalLine = NULL;

   while(!mcc_FileBufferEOFReached(fileBuffer))
   {
      logicalLine = mcc_FileBufferGetNextLogicalLine(fileBuffer);
      if ((logicalLine->length > 0) && (logicalLine->index < logicalLine->length))
      {
         mcc_TokeniseLine(logicalLine, fileBuffer, iter);
      }
      mcc_AddEndOfLineToken(
         logicalLine->index+1,
         mcc_GetFileBufferCurrentLineNo(fileBuffer),
         mcc_GetFileBufferFileNumber(fileBuffer),
         iter);
   }
   mcc_DeleteFileBuffer(fileBuffer);
}

//by providing a logical line, we are guaranteed to only have whole tokens.
static void mcc_TokeniseLine(mcc_LogicalLine_t *line,
                             mcc_FileBuffer_t *fileBuffer,
                             mcc_TokenListIterator_t *iter)
{
   MCC_OPERATOR current_operator = OP_NONE;
   MCC_SYMBOL current_symbol = SYM_NONE;
   mcc_Token_t *token = NULL;
   static bool_t insideMultiLineComment = FALSE;

   if (!insideMultiLineComment)
   {
      handle_whitespace(line, fileBuffer, iter);
   }

   while(line->index < line->length)
   {
      if ((line->string[line->index] == '/' &&
           line->string[line->index+1] == '*') || insideMultiLineComment)
      {
         //If this is the first line/opening part of the comment, handle it.
         if (!insideMultiLineComment)
         {
            insideMultiLineComment = TRUE;
            line->index += 2;
         }
         while(line->index < line->length)
         {
            if (line->index < (line->length-1) &&
                (line->string[line->index] == '*' && line->string[line->index+1] == '/'))
            {
               line->index += 2;
               insideMultiLineComment = FALSE;
               break;
            }
            else
            {
               line->index++;
            }
         }
      }
      else if (line->string[line->index] == '/' &&
               line->string[line->index+1] == '/')
      {
         //consider creating a token for the comment, such that we can preserve them in preprocessed output
         while(line->index < line->length)
            line->index++;
      }
      else if (line->string[line->index] == '#')
      {
         line->index++;
         if (line->string[line->index] == '#')
         {
            token = mcc_CreateToken("#", 1, TOK_PP_DIRECTIVE,
                                    PP_JOIN,
                                    line->index+1,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                    mcc_GetFileBufferFileNumber(fileBuffer));
            mcc_InsertToken(token, iter);
            line->index++;
            token = NULL;
         }
         else
         {
            PREPROC_DIRECTIVE pp_dir;
            handle_whitespace(line, fileBuffer, iter);
            pp_dir = mcc_GetPreprocessorDirective(line);
            MCC_ASSERT(pp_dir != PP_NONE);
            token = mcc_CreateToken(preprocessor_directives[pp_dir], 
                                    pp_strlens[pp_dir], TOK_PP_DIRECTIVE,
                                    pp_dir,
                                    line->index+1,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                    mcc_GetFileBufferFileNumber(fileBuffer));
            line->index += pp_strlens[pp_dir];
            if (pp_dir == PP_INCLUDE)
            {
               mcc_InsertToken(token, iter);
               token = NULL;
               handle_pp_include_filename(line, fileBuffer, iter);
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
                                     '"', TOK_STR_CONST, iter);
         }
         else if (current_symbol == SYM_SINGLE_QUOTE)
         {
            handle_string_char_const(line, fileBuffer,
                                     '\'', TOK_CHAR_CONST, iter);            
         }
         else
         {
            token = mcc_CreateToken(symbols[current_symbol],
                                    symbol_strlens[current_symbol],
                                    TOK_SYMBOL,
                                    current_symbol,
                                    line->index+1,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                    mcc_GetFileBufferFileNumber(fileBuffer));
            line->index += symbol_strlens[current_symbol];
         }
      }
      else if ((current_operator = mcc_GetOperator(line)) != OP_NONE)
      {
         // We can't tell the difference between * and * without a little
         // more context.  (Dereference and multiply)
         if (current_operator == OP_DEREFERENCE)
         {
            const mcc_Token_t *temp = mcc_PeekPreviousNonWhitespaceToken(iter);
            if (temp &&
                  (temp->tokenType == TOK_NUMBER ||
                   temp->tokenType == TOK_IDENTIFIER ||
                   (temp->tokenType == TOK_SYMBOL && temp->tokenIndex == SYM_CLOSE_PAREN)))
            {
               current_operator = OP_MULTIPLY;
            }
         }
         token = mcc_CreateToken(operators[current_operator],
                                 operator_strlens[current_operator], 
                                 TOK_OPERATOR,
                                 current_operator,
                                 line->index+1,
                                 mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                 mcc_GetFileBufferFileNumber(fileBuffer));
         line->index += operator_strlens[current_operator];
      }
      else if (isNumber(line->string[line->index]))
      {
         bool_t isUnsigned = FALSE;
         bool_t isLong = FALSE;
         bool_t isDouble = FALSE;
         bool_t hadSuffix = FALSE;
         int numLen = 1;
         while (isNumber(line->string[line->index + numLen]) &&
                 line->index + numLen < line->length)
         {
            numLen++;
         }
         if (toupper(line->string[line->index + numLen]) == 'E')
         {
            while (isNumber(line->string[line->index + numLen]) &&
                   line->index + numLen < line->length)
            {
               numLen++;
            }
         }
         if (toupper(line->string[line->index + numLen]) == 'U')
         {
            isUnsigned = TRUE;
            hadSuffix = TRUE;
         }
         else if (toupper(line->string[line->index + numLen]) == 'L')
         {
            isLong = TRUE;
            hadSuffix = TRUE;
         }
         else if (line->string[line->index + numLen] == '.')
         {
            while (isNumber(line->string[line->index + numLen]) &&
                   line->index + numLen < line->length)
            {
               numLen++;
            }
            if (toupper(line->string[line->index + numLen]) == 'F' ||
                toupper(line->string[line->index + numLen]) == 'D')
            {
               isDouble = TRUE;
               hadSuffix = TRUE;
            }
         }
         
         token = mcc_CreateToken(&line->string[line->index], numLen,
                                 TOK_NUMBER,
                                 TOK_UNSET_INDEX,
                                 line->index+1,
                                 mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                 mcc_GetFileBufferFileNumber(fileBuffer));
         if (isDouble)
         {
            token->number.number.float_d = strtod(token->text, NULL);
            token->number.numberType = DOUBLE;
         }
         else if (isUnsigned)
         {
            token->number.number.integer_u = strtoul(token->text, NULL, 10);
            token->number.numberType = UNSIGNED_INT;
         }
         else if (isLong)
         {
            // Technically, this isn't true
            token->number.number.integer_s = strtol(token->text, NULL, 10);
            token->number.numberType = SIGNED_INT;
         }
         else 
         {
            token->number.number.integer_s = strtol(token->text, NULL, 10);
            token->number.numberType = SIGNED_INT;
         }
         line->index += numLen;
         if (hadSuffix)
         {
            line->index++;
         }
      }
      else if (isWordChar(line->string[line->index]))
      {
         MCC_KEYWORD keyword = mcc_GetKeyword(line);
         if (keyword != KEY_NONE)
         {
            token = mcc_CreateToken(keywords[keyword], keyword_strlens[keyword],
                                    TOK_KEYWORD,
                                    keyword,
                                    line->index+1,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                    mcc_GetFileBufferFileNumber(fileBuffer));
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
                                    TOK_UNSET_INDEX,
                                    line->index+1,
                                    mcc_GetFileBufferCurrentLineNo(fileBuffer),
                                    mcc_GetFileBufferFileNumber(fileBuffer));
            line->index += identLen;
         }
      }
      else
      {
         mcc_PrettyError(mcc_GetFileBufferFilename(fileBuffer),
                         mcc_GetFileBufferCurrentLineNo(fileBuffer),
                         line->index,
                         "Not a recognised character: '%c'.  This is probably a bug in the tokeniser.\n",
                         line->string[line->index]);
      }
     
      if (token != NULL)
      {
         mcc_InsertToken(token, iter);
         token = NULL;
      }
      handle_whitespace(line, fileBuffer, iter);
   }
}

