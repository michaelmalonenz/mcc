#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "macro.h"

#define SYSTEM_INCLUDE_OPENER '<'
#define SYSTEM_INCLUDE_TERMINATOR '>'
#define LOCAL_INCLUDE_OPENER '"'
#define LOCAL_INCLUDE_TERMINATOR '"'

/* The order here, is my guess at the relative frequency of each directive's use - so we can match
   earlier in the list, and hopefully speed up the pre-processing portion a little bit */
enum pp_directives { PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF, PP_IF, PP_ENDIF, PP_ELSE,
                     PP_ELIF, PP_UNDEF, PP_ERROR, PP_PRAGMA, NUM_PREPROCESSOR_DIRECTIVES };

static const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
                                                                            "endif", "else", "elif", "undef", "error", "pragma" };

typedef void (preprocessorDirectiveHandler_t)(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);

static void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleIfdef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleIfndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleIf(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleEndif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleElse(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleElif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleUndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handleError(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static void handlePragma(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);

static void SearchPreprocessorDirectives(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly);
static mcc_StringBuffer_t *GetMacroIdentifier(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static mcc_LogicalLine_t *DealWithComments(mcc_LogicalLine_t* line, mcc_FileBuffer_t *fileBuffer);

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleInclude, &handleDefine, &handleIfdef,
                                                                                   &handleIfndef, &handleIf, &handleEndif,
                                                                                   &handleElse, &handleElif, &handleUndef,
                                                                                   &handleError, &handlePragma };

static void handleDefinedConditional(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer,
                                     bool_t parseOnly, bool_t isTrue);

static FILE *outputFile;
static bool_t insideConditional;
static bool_t conditionalIsTrue;

static void SearchPreprocessorDirectives(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   int i;
   SkipWhiteSpace(line);
   if (line->string[line->index] == '#')
   {
      line->index++;
      for(i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
      {
         if (strncmp((char *)&line->string[line->index], preprocessor_directives[i],
                     strlen(preprocessor_directives[i])) == 0)
         {
            line->index += strlen(preprocessor_directives[i]);
            ppHandlers[i](line, fileBuffer, parseOnly);
            return;
         }
      }
   }
}

static mcc_StringBuffer_t *GetMacroIdentifier(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer)
{
   mcc_StringBuffer_t *idBuffer = mcc_CreateStringBuffer();
   SkipWhiteSpace(line);
   if (!isWordChar(line->string[line->index]))
   {
      mcc_Error("Illegal macro identifier at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   while( (line->index < line->length) && (isWordChar(line->string[line->index])) )
   {
      mcc_StringBufferAppendChar(idBuffer, line->string[line->index]);
      line->index++;
   }
   mcc_StringBufferAppendChar(idBuffer, '\0');
   return idBuffer;
}


// only handles c++ style comments just now - like this one!
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

void mcc_PreprocessFile(const char *inFilename, FILE *outFile)
{
   mcc_FileBuffer_t *fileBuffer = mcc_CreateFileBuffer(inFilename);
   mcc_LogicalLine_t *logicalLine = NULL;
   
   outputFile = outFile;

   while(!mcc_FileBufferEOFReached(fileBuffer))
   {
      logicalLine = mcc_FileBufferGetNextLogicalLine(fileBuffer);
      if (logicalLine->length > 0)
      {
         //doMacroReplacement(logicalLine);
         logicalLine = DealWithComments(logicalLine, fileBuffer);
         if (logicalLine->index == logicalLine->length)
         {
            continue;
         }
         SearchPreprocessorDirectives(logicalLine, fileBuffer, FALSE);
      }
   }
   mcc_DeleteFileBuffer(fileBuffer);
}

static void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   mcc_StringBuffer_t *fileInclude = mcc_CreateStringBuffer();
   char terminator;
   if (parseOnly)
      return;

   SkipWhiteSpace(line);
   if (line->string[line->index] == LOCAL_INCLUDE_OPENER)
   {
      terminator = LOCAL_INCLUDE_TERMINATOR;
   }
   else if (line->string[line->index] == SYSTEM_INCLUDE_OPENER)
   {
      terminator = SYSTEM_INCLUDE_TERMINATOR;
   }
   else
   {
      mcc_Error("Expected '%c' or '%c' in %s:%d\n",
                LOCAL_INCLUDE_OPENER,
                SYSTEM_INCLUDE_OPENER,
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   while(line->string[++(line->index)] != terminator)
   {
      if (line->index < line->length)
      {
         mcc_StringBufferAppendChar(fileInclude, line->string[line->index]);
      }
      else
      {
         mcc_Error("Expected '%c' in %s:%d\n", 
                   terminator,
                   mcc_GetFileBufferFilename(fileBuffer),
                   mcc_GetFileBufferCurrentLineNo(fileBuffer));
      }
   }
   mcc_StringBufferAppendChar(fileInclude, '\0');
   if (terminator == LOCAL_INCLUDE_TERMINATOR)
   {
      mcc_PreprocessFile(mcc_FindLocalInclude((const char *)mcc_StringBufferGetString(fileInclude)),
                         outputFile);
   }
   else
   {
//	mcc_PreprocessFile(mcc_FindSystemInclude((const char *)mcc_StringBufferGetString(fileInclude)),
//      outputFile);
   }
   mcc_DeleteStringBuffer(fileInclude);
}

//currently doesn't handle function-like macros
static void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   mcc_StringBuffer_t *idBuffer = GetMacroIdentifier(line, fileBuffer);
   char *macro_value = NULL;
   if (parseOnly)
      return;
   SkipWhiteSpace(line);
   if (line->index < line->length)
   {
      macro_value = &line->string[line->index];
   }
   mcc_DefineMacro(mcc_StringBufferGetString(idBuffer), macro_value);
   mcc_DeleteStringBuffer(idBuffer);
}

static void handleUndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   mcc_StringBuffer_t *idBuffer;
   if (parseOnly)
      return;
   idBuffer = GetMacroIdentifier(line, fileBuffer);
   mcc_UndefineMacro(mcc_StringBufferGetString(idBuffer));
   mcc_DeleteStringBuffer(idBuffer);
}

static void handleError(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   if (parseOnly)
      return;
   SkipWhiteSpace(line);
   mcc_Error("Error: %s \nat %s:%d\n", &line->string[line->index],
             mcc_GetFileBufferFilename(fileBuffer),
             mcc_GetFileBufferCurrentLineNo(fileBuffer));
}

static void handleDefinedConditional(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer,
                                     bool_t parseOnly, bool_t isTrue)
{
   mcc_StringBuffer_t *idBuffer;
   idBuffer = GetMacroIdentifier(line, fileBuffer);
   SkipWhiteSpace(line);
   if (line->index != line-> length)
   {
      mcc_Error("Extra characters after Macro after conditional at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   insideConditional = TRUE;
   if (mcc_ResolveMacro(mcc_StringBufferGetString(idBuffer)))
   {
      conditionalIsTrue = isTrue;
   }
   else
   {
      conditionalIsTrue = !isTrue;
   }
   line = mcc_FileBufferGetNextLogicalLine(fileBuffer);
   while (!mcc_FileBufferEOFReached(fileBuffer))
   {
      line = DealWithComments(line, fileBuffer);
      SkipWhiteSpace(line);
      if ( (line->length > 0) && (line->string[line->index] == '#') )
      {
         if (strncmp((char *)&line->string[line->index+1],
                     preprocessor_directives[PP_ENDIF],
                     strlen(preprocessor_directives[PP_ENDIF])) == 0)
         {
            line->index += (strlen(preprocessor_directives[PP_ENDIF]) + 1);
            ppHandlers[PP_ENDIF](line, fileBuffer, parseOnly);
            mcc_DeleteStringBuffer(idBuffer);
            return;
         }
         else
         {
            SearchPreprocessorDirectives(line, fileBuffer, !conditionalIsTrue);
         }
      }
      line = mcc_FileBufferGetNextLogicalLine(fileBuffer);
   }
   //save the filename and line number
   mcc_Error("Expected #endif for conditional '%s'\n",
             mcc_StringBufferGetString(idBuffer));
   
}

static void handleIfdef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   handleDefinedConditional(line, fileBuffer, parseOnly, TRUE);
}

static void handleIfndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t parseOnly)
{
   handleDefinedConditional(line, fileBuffer, parseOnly, FALSE);
}

static void handleIf(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(parseOnly))
{
   insideConditional = TRUE;
}

static void handleEndif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t UNUSED(parseOnly))
{
   SkipWhiteSpace(line);
   if (line->index != line->length)
   {
      mcc_Error("Extra characters '%s' after endif at %s:%d\n",
                &line->string[line->index],
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   if (insideConditional)
   {
      insideConditional = FALSE;
   }
}

static void handleElse(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t UNUSED(parseOnly))
{
   SkipWhiteSpace(line);
   if (line->index != line->length)
   {
      mcc_Error("Extra Characters after else at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   if (insideConditional)
   {
      conditionalIsTrue = !conditionalIsTrue;
   }
   else
   {
      mcc_Error("Else without If at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
}

static void handleElif(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(parseOnly)) {}

//What shall I do with #pragmas???
static void handlePragma(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(parseOnly)) {}
