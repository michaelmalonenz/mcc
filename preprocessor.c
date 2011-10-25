#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "macro.h"
#include "tokens.h"

#define SYSTEM_INCLUDE_OPENER '<'
#define SYSTEM_INCLUDE_TERMINATOR '>'
#define LOCAL_INCLUDE_OPENER '"'
#define LOCAL_INCLUDE_TERMINATOR '"'

const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
                                                                            "endif", "else", "elif", "undef", "error", "pragma" };

size_t pp_strlens[NUM_PREPROCESSOR_DIRECTIVES];
static bool_t initialised;

#ifdef MCC_DEBUG
#define HANDLER_LINKAGE extern
#else
#define HANDLER_LINKAGE static
#endif

typedef void (preprocessorDirectiveHandler_t)(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);

HANDLER_LINKAGE void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleIfdef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleIfndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleIf(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleEndif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleElse(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleElif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleUndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handleError(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
HANDLER_LINKAGE void handlePragma(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);

static void SearchPreprocessorDirectives(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip);
static mcc_StringBuffer_t *GetMacroIdentifier(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
HANDLER_LINKAGE mcc_LogicalLine_t *DealWithComments(mcc_LogicalLine_t* line, mcc_FileBuffer_t *fileBuffer);

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleInclude, &handleDefine, &handleIfdef,
                                                                                   &handleIfndef, &handleIf, &handleEndif,
                                                                                   &handleElse, &handleElif, &handleUndef,
                                                                                   &handleError, &handlePragma };

HANDLER_LINKAGE void handleDefinedConditional(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer,
                                     bool_t skip, bool_t isTrue);

static FILE *outputFile;

PREPROC_DIRECTIVE mcc_GetPreprocessorDirective(mcc_LogicalLine_t *line)
{
   int i;
   if (!initialised)
   {
      for (i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
      {
         pp_strlens[i] = strlen(preprocessor_directives[i]);
      }
      initialised = TRUE;
   }

   for (i = 0; i < NUM_PREPROCESSOR_DIRECTIVES; i++)
   {
      if (strncmp(&line->string[line->index], preprocessor_directives[i], pp_strlens[i]) == 0)
      {
         return i;
      }
   }
   return PP_NONE;
}
void SearchPreprocessorDirectives(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
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
            ppHandlers[i](line, fileBuffer, skip);
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
HANDLER_LINKAGE mcc_LogicalLine_t *DealWithComments(mcc_LogicalLine_t* line, mcc_FileBuffer_t *fileBuffer)
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
   mcc_DeleteAllMacros();
}

HANDLER_LINKAGE void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   mcc_StringBuffer_t *fileInclude = mcc_CreateStringBuffer();
   char terminator;
   if (skip)
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
HANDLER_LINKAGE void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   mcc_StringBuffer_t *idBuffer = GetMacroIdentifier(line, fileBuffer);
   char *macro_value = NULL;
   if (skip)
      return;
   SkipWhiteSpace(line);
   if (line->index < line->length)
   {
      macro_value = &line->string[line->index];
   }
   mcc_DefineMacro(mcc_StringBufferGetString(idBuffer), macro_value);
   mcc_DeleteStringBuffer(idBuffer);
}

HANDLER_LINKAGE void handleUndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   mcc_StringBuffer_t *idBuffer;
   if (skip)
      return;
   idBuffer = GetMacroIdentifier(line, fileBuffer);
   mcc_UndefineMacro(mcc_StringBufferGetString(idBuffer));
   mcc_DeleteStringBuffer(idBuffer);
}

HANDLER_LINKAGE void handleError(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   if (skip)
      return;
   SkipWhiteSpace(line);
   mcc_Error("Error: %s \nat %s:%d\n", &line->string[line->index],
             mcc_GetFileBufferFilename(fileBuffer),
             mcc_GetFileBufferCurrentLineNo(fileBuffer));
}

HANDLER_LINKAGE void handleDefinedConditional(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer,
                                     bool_t skip, bool_t isPositive)
{
   mcc_StringBuffer_t *idBuffer;
   bool_t conditionalIsTrue;
   idBuffer = GetMacroIdentifier(line, fileBuffer);
   SkipWhiteSpace(line);
   if (line->index != line->length)
   {
      mcc_Error("Extra characters after Macro after conditional at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
   if (mcc_ResolveMacro(mcc_StringBufferGetString(idBuffer)))
   {
      conditionalIsTrue = isPositive;
   }
   else
   {
      conditionalIsTrue = !isPositive;
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
            ppHandlers[PP_ENDIF](line, fileBuffer, skip);
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

HANDLER_LINKAGE void handleIfdef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   handleDefinedConditional(line, fileBuffer, skip, TRUE);
}

HANDLER_LINKAGE void handleIfndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   handleDefinedConditional(line, fileBuffer, skip, FALSE);
}

HANDLER_LINKAGE void handleIf(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(skip))
{
}

HANDLER_LINKAGE void handleEndif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t UNUSED(skip))
{
   SkipWhiteSpace(line);
   if (line->index != line->length)
   {
      mcc_Error("Extra characters '%s' after endif at %s:%d\n",
                &line->string[line->index],
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
}

HANDLER_LINKAGE void handleElse(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer, bool_t skip)
{
   SkipWhiteSpace(line);
   if (skip)
      return;
   if (line->index != line->length)
   {
      mcc_Error("Extra Characters after else at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
/*   if (insideConditional)
   {
      conditionalIsTrue = !conditionalIsTrue;
   }
   else
   {
      mcc_Error("Else without If at %s:%d\n",
                mcc_GetFileBufferFilename(fileBuffer),
                mcc_GetFileBufferCurrentLineNo(fileBuffer));
   }
*/ //need to make this while thing use the stack to figure out where it should be
   //otherwise nested conditionals will get lost pretty quickly
}

HANDLER_LINKAGE void handleElif(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(skip)) {}

//What shall I do with #pragmas???
HANDLER_LINKAGE void handlePragma(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer), bool_t UNUSED(skip)) {}
