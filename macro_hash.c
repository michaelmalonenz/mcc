#include "config.h"
#include "macro.h"
#include "macro_private.h"

#if MCC_USE_HASH_TABLE_FOR_MACROS

void mcc_DeleteAllMacros(void)
{
}

void mcc_DefineMacro(const char UNUSED(*text), char UNUSED(*value))
{
}

void mcc_UndefineMacro(const char UNUSED(*text))
{
}

mcc_Macro_t *mcc_ResolveMacro(const char UNUSED(*text))
{
   mcc_Macro_t *result = NULL;
   return result;
}

void mcc_DoMacroReplacement(mcc_LogicalLine_t UNUSED(*line))
{
}

#endif
