#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc.h"
#include "fileBuffer.h"
#include "stringBuffer.h"
#include "macro.h"

/* The order here, is my guess at the relative frequency of each directive's use - so we can match
   earlier in the list, and hopefully speed up the pre-processing portion a little bit */
enum pp_directives { PP_INCLUDE, PP_DEFINE, PP_IFDEF, PP_IFNDEF, PP_IF, PP_ENDIF, PP_ELSE,
					 PP_ELIF, PP_UNDEF, PP_ERROR, PP_PRAGMA, NUM_PREPROCESSOR_DIRECTIVES };

static const char *preprocessor_directives[NUM_PREPROCESSOR_DIRECTIVES] = { "include", "define", "ifdef", "ifndef", "if",
																			"endif", "else", "elif", "undef", "error", "pragma" };

typedef void (preprocessorDirectiveHandler_t)(mcc_LogicalLine_t *line);

static void handleInclude(mcc_LogicalLine_t *line);
static void handleDefine(mcc_LogicalLine_t *line);

static preprocessorDirectiveHandler_t *ppHandlers[NUM_PREPROCESSOR_DIRECTIVES] = { &handleInclude, &handleDefine};


static void searchPreprocessorDirectives(mcc_LogicalLine_t *line)
{
	int j;
	for(line->index = 0; line->index < line->length; line->index++)
	{
		if(!isNonBreakingWhiteSpace(line->string[line->index]))
        {
			if (line->string[line->index] == '#')
			{
				for(j = 0; j < NUM_PREPROCESSOR_DIRECTIVES; j++)
				{
					if (strncmp((char *)&line->string[line->index+1], preprocessor_directives[j],
								(line->length - line->index)) == 0)
					{
                        ppHandlers[j](line);
						printf("%s\n", line->string);
						return;
					}
				}
			}
			else
			{
				return;
			}
		}
	}
}

void mcc_PreprocessFile(const char *inFilename, FILE UNUSED(*outFile))
{
	mcc_FileBuffer_t *fileBuffer = mcc_CreateFileBuffer(inFilename);
	mcc_LogicalLine_t *logicalLine = NULL;

	while(!mcc_FileBufferEOFReached(fileBuffer))
	{
		logicalLine = mcc_FileBufferGetNextLogicalLine(fileBuffer);
//		printf("%s\n", logicalLine);
		if (logicalLine->length > 0)
		{
//			doMacroReplacement(logicalLine);
			searchPreprocessorDirectives(logicalLine);
		}
	}

	mcc_DeleteFileBuffer(fileBuffer);
}

static inline void skipWhiteSpace(mcc_LogicalLine_t *line)
{
    while( (line->index < line->length) && (isNonBreakingWhiteSpace(line->string[line->index])) )
        line->index++;
}

static void handleInclude(mcc_LogicalLine_t UNUSED(*line))
{
}

static void handleDefine(mcc_LogicalLine_t *line)
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
