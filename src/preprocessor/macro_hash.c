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
#include "config.h"
#include "macro.h"
#include "macro_private.h"
#include "liberal.h"

#include <stdint.h>
#include <string.h>

static eral_HashTable_t *macro_table;

static void mcc_DefineMacroAsNumber(const char *ident, int value)
{
    mcc_TokenList_t *tokens = mcc_TokenListCreate();
    mcc_Number_t number = {
        .number = {.integer_s = value},
        .numberType = SIGNED_INT};
    mcc_TokenListAppend(tokens, mcc_CreateNumberToken(&number, 0, 0, 255));
    mcc_DefineMacro(ident, tokens, NULL, false);
}

void mcc_InitialiseMacros(void)
{
    macro_table = eral_HashTableCreateSize(8192);
    mcc_DefineMacro("__STDC__", NULL, NULL, false);
    mcc_DefineMacroAsNumber("__x86_64__", 1);
    mcc_DefineMacroAsNumber("__LP64__", 1);
    mcc_DefineMacroAsNumber("__WORDSIZE", 64);
    mcc_DefineMacroAsNumber("__WCHAR_MAX__", 0x7fffffff);
    mcc_DefineMacroAsNumber("__WCHAR_MIN__", -0x7fffffff - 1);
#if MCC_C99_COMPATIBLE
    mcc_DefineMacroAsNumber("__STDC_VERSION__", 199901L);
#endif
}

static void delete_macro_pointer(uintptr_t macro)
{
    delete_macro((mcc_Macro_t *) macro);
}

void mcc_DeleteAllMacros(void)
{
    eral_HashTableDelete(macro_table, delete_macro_pointer);
    macro_table = NULL;
}

void mcc_DefineMacro(const char *text, mcc_TokenList_t *value, mcc_TokenList_t *arguments, bool variadic)
{
    eral_HashTableInsert(macro_table, text, strlen(text), (uintptr_t) create_macro(text, value, arguments, variadic));
}

void mcc_UndefineMacro(const char *text)
{
    mcc_Macro_t *temp = (mcc_Macro_t *) eral_HashTableRemove(macro_table, text, strlen(text));
    if (temp)
    {
        delete_macro(temp);
    }
}

mcc_Macro_t *mcc_ResolveMacro(const char *text)
{
    mcc_Macro_t *result = (mcc_Macro_t *) eral_HashTableFind(macro_table, text, strlen(text));
    return result;
}
