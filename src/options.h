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
#ifndef _MCC_OPTIONS_H_
#define _MCC_OPTIONS_H_

#include <stdint.h>

#include "mcc.h"
#include "liberal.h"

enum
{
  PREPROCESS = 1,
  COMPILE = (1 << 1),
  LINK = (1 << 2)
};

typedef struct
{
  uint32_t stages;
  eral_List_t *filenames;
  const char *outputFilename;
} mcc_Options_t;

extern mcc_Options_t mcc_global_options;

void mcc_ParseOptions(int argc, char **argv);

eral_ListIterator_t *mcc_OptionsFileListGetIterator(void);

/**
 * A method mainly for unit testing.  It destroys the dynamic memory associated
 * with the option parameters, which in the real world, need to be in use for
 * the lifetime of the program.
 */
void mcc_TearDownOptions(void);

#endif /* _MCC_OPTIONS_H_ */
