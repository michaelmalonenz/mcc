#ifndef _MCC_MACRO_EXPANSION_H_
#define _MCC_MACRO_EXPANSION_H_
#include "macro.h"
#include "preprocessor_private.h"

mcc_TokenList_t *expandMacroTokens(mcc_Macro_t *macro);

mcc_TokenList_t *expandMacroFunctionTokens(preprocessor_t *preprocessor, mcc_Macro_t *macro);

#endif
