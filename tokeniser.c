#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "mcc.h"
#include "fileBuffer.h"

#include "stringBuffer.h"

//by providing a logical line, we are guaranteed to only have whole tokens.
void mcc_TokeniseLine(mcc_LogicalLine_t* line, mcc_FileBuffer_t UNUSED(*fileBuffer))
{
   SkipWhiteSpace(line);
	
}

