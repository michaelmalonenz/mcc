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
#include <string.h>

#include "options.h"

mcc_Options_t mcc_global_options;

void mcc_ParseOptions(int argc, char **argv)
{
   int i;
   mcc_global_options.filenames = eral_ListCreate();
   mcc_global_options.stages = (PREPROCESS &
                                COMPILE &
                                LINK);

   for (i = 1; i < argc; i++)
   {
      if (strncmp(argv[i], "-o", strlen(argv[i])) == 0)
      {
         i++;
         if (i == argc)
         {
            mcc_Error("The -o flag requires an output file argument\n");
         }
         mcc_global_options.outputFilename = argv[i];
      }
      else if (strncmp(argv[i], "-E", strlen(argv[i])) == 0)
      {
         mcc_global_options.stages = PREPROCESS;
      }
      else if (strncmp(argv[i], "-I", 2) == 0)
      {
         mcc_FileOpenerLocalIncAppendDir(&argv[i][2]);
      }
      else if (argv[i][0] == '-')
      {
         //do nothing;  Silently drop all unused arguments
      }
      else
      {
         eral_ListAppendData(mcc_global_options.filenames, (uintptr_t) argv[i]);
      }
   }
}

eral_ListIterator_t *mcc_OptionsFileListGetIterator(void)
{
   return eral_ListGetIterator(mcc_global_options.filenames);
}

void mcc_TearDownOptions(void)
{
   eral_ListDelete(mcc_global_options.filenames, NULL);
}
