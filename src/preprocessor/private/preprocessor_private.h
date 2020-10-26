/**
 Copyright (c) 2019, Michael Malone
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
#ifndef _PREPROCESSOR_PRIVATE_H_
#define _PREPROCESSOR_PRIVATE_H_
#include "tokenList.h"

typedef struct preprocessor {
   mcc_TokenListIterator_t *tokenListIter;
   const mcc_Token_t *currentToken;
   mcc_TokenList_t *output;
} preprocessor_t;

void getToken(preprocessor_t *preprocessor);

void maybeGetWhiteSpaceToken(preprocessor_t *preprocessor);

void handlePreprocessorDirective(preprocessor_t *preprocessor);

mcc_TokenList_t *resolveMacroTokens(preprocessor_t *preprocessor, const char *macroText);

void emitToken(preprocessor_t *preprocessor);

typedef void (preprocessorDirectiveHandler_t)(preprocessor_t *preprocessor);

void handleInclude(preprocessor_t *preprocessor);
void handleDefine(preprocessor_t *preprocessor);
void handleIfdef(preprocessor_t *preprocessor);
void handleIfndef(preprocessor_t *preprocessor);
void handleIf(preprocessor_t *preprocessor);
void handleEndif(preprocessor_t *preprocessor);
void handleElse(preprocessor_t *preprocessor);
void handleElif(preprocessor_t *preprocessor);
void handleUndef(preprocessor_t *preprocessor);
void handleError(preprocessor_t *preprocessor);
void handlePragma(preprocessor_t *preprocessor);
void handleJoin(preprocessor_t *preprocessor);
void handleWarning(preprocessor_t *preprocessor);
void handleStringify(preprocessor_t *preprocessor);

#endif /* _PREPROCESSOR_PRIVATE_H_ */
