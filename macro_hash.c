#include "config.h"
#include "macro.h"
#include "macro_private.h"

#if MCC_USE_HASH_TABLE_FOR_MACROS

/* I'm told that a x^2 size is good for speed and a hash function
 * which produces uniform results is better than an average hash
 * and a prime number hash table size.
 */
#define HASH_TABLE_LENGTH 8192

static mcc_macro_t *macro_table[HASH_TABLE_LENGTH];

void mcc_DeleteAllMacros(void)
{
}

void mcc_DefineMacro(const char *text, char UNUSED(*value))
{
    uint32_t hashVal = hash(text, strlen(text));
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
