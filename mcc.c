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
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdlib.h>

#include "mcc.h"
#include "tokenList.h"
#include "toolChainCommands.h"
#include "options.h"
#include "macro.h"

int main(int argc, char **argv)
{
   mcc_TokenListIterator_t *tokenListIter;
   mcc_ListIterator_t *fileIter;
   char *currentFile;
   
   if (argc <= 1)
   {
      mcc_Error("No input files\n");
   }

   mcc_ParseOptions(argc, argv);

   mcc_FileOpenerInitialise();
   mcc_InitialiseMacros();

   fileIter = mcc_OptionsFileListGetIterator();
   currentFile = (char *) mcc_ListGetNextData(fileIter);
   while (currentFile != NULL)
   {
      tokenListIter = mcc_TokenListGetIterator();
      fprintf(stderr, "Tokenising %s...\n", currentFile);
      mcc_TokeniseFile(currentFile, tokenListIter);
      fprintf(stderr, "Preprocessing %s...\n", currentFile);
      mcc_List_t *output = mcc_PreprocessCurrentTokens();
      mcc_TokenListDeleteIterator(tokenListIter);
      if (mcc_global_options.stages == PREPROCESS)
      {
         mcc_WriteTokensToOutputFile(output);
      }
      mcc_TokenListDeleteStandalone(output);
      mcc_FreeTokens();
      mcc_DeleteAllMacros();
      currentFile = (char *) mcc_ListGetNextData(fileIter);
   }

   mcc_FileOpenerDelete();

   mcc_TearDownOptions();

   return EXIT_SUCCESS;
} 
