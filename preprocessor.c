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

typedef void (preprocessorDirectiveHandler_t)(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);

static void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleIfdef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleIfndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleIf(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleEndif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleElse(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleElif(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleUndef(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handleError(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);
static void handlePragma(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer);

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleInclude, &handleDefine, &handleIfdef,
																				   &handleIfndef, &handleIf, &handleEndif,
																				   &handleElse, &handleElif, &handleUndef,
																				   &handleError, &handlePragma };

static FILE *outputFile;

static inline void skipWhiteSpace(mcc_LogicalLine_t *line)
{
    while( (line->index < line->length) && (isNonBreakingWhiteSpace(line->string[line->index])) )
        line->index++;
}

static void searchPreprocessorDirectives(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer)
{
	int j;
    skipWhiteSpace(line);
    if (line->string[line->index] == '#')
    {
        line->index++;
        for(j = 0; j < NUM_PREPROCESSOR_DIRECTIVES; j++)
        {
            if (strncmp((char *)&line->string[line->index], preprocessor_directives[j],
                        strlen(preprocessor_directives[j])) == 0)
            {
                printf("%s\n", line->string);
				line->index += strlen(preprocessor_directives[j]);
                ppHandlers[j](line, fileBuffer);
                return;
            }
        }
    }
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
//            printf("%s\n", logicalLine->string);
//			doMacroReplacement(logicalLine);
			searchPreprocessorDirectives(logicalLine, fileBuffer);
		}
	}

	mcc_DeleteFileBuffer(fileBuffer);
}

static void handleInclude(mcc_LogicalLine_t *line, mcc_FileBuffer_t *fileBuffer)
{
	mcc_StringBuffer_t *fileInclude = mcc_CreateStringBuffer();
	char terminator;
	skipWhiteSpace(line);
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
//		mcc_PreprocessFile(mcc_FindSystemInclude((const char *)mcc_StringBufferGetString(fileInclude)),
//						   outputFile);
	}
	mcc_DeleteStringBuffer(fileInclude);
}

static void handleDefine(mcc_LogicalLine_t *line, mcc_FileBuffer_t UNUSED(*fileBuffer))
{
    mcc_StringBuffer_t *idBuffer = mcc_CreateStringBuffer();
    //find the start of the Macro identifier
    skipWhiteSpace(line);
    while( (line->index < line->length) && (isWordChar(line->string[line->index])) )
    {
        mcc_StringBufferAppendChar(idBuffer, line->string[line->index]);
    }
    skipWhiteSpace(line);
    mcc_DefineMacro((char *)mcc_DestroyBufferNotString(idBuffer),
                    (char *)&line->string[line->index]);
}

static void handleIfdef(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleIfndef(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleIf(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleEndif(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleElse(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleElif(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleUndef(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handleError(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
static void handlePragma(mcc_LogicalLine_t UNUSED(*line), mcc_FileBuffer_t UNUSED(*fileBuffer)) {}
