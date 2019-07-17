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
#ifndef MCC_MACRO_H
#define MCC_MACRO_H

#include "liberal.h"
#include "config.h"
#include "tokenList.h"

typedef struct macro {
   char *text;
   char *value;
   mcc_TokenList_t *tokens;
   mcc_TokenList_t *arguments;
   bool_t is_function;
   struct macro *next;
}mcc_Macro_t;

typedef struct macroParameter {
   const mcc_Token_t *argument;
   mcc_Token_t *parameter;
}mcc_MacroParameter_t;

void mcc_InitialiseMacros(void);

void mcc_DeleteAllMacros(void);

void mcc_DefineMacro(const char *text, mcc_TokenList_t *tokens, mcc_TokenList_t *arguments);

void mcc_UndefineMacro(const char *text);

bool_t mcc_IsMacroDefined(const char *text);

mcc_Macro_t *mcc_ResolveMacro(const char *text);

mcc_MacroParameter_t *mcc_MacroParameterCreate(void);
void mcc_MacroParameterDelete(uintptr_t param);
#endif /* MCC_MACRO_H */
