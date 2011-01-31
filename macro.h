#ifndef MCC_MACRO_H
#define MCC_MACRO_H

#include "fileBuffer.h"
#include "config.h"

typedef struct macro {
   char *text;
   char *value;
   //char **args; ??
#if MCC_USE_HASH_TABLE_FOR_MACROS
   struct macro *next;
#elif MCC_USE_B_TREE_FOR_MACROS
   struct macro *left;
   struct macro *right;
#endif
}mcc_Macro_t;

void mcc_DeleteAllMacros(void);

void mcc_DefineMacro(const char *text, char *value);

void mcc_UndefineMacro(const char *text);

mcc_Macro_t *mcc_ResolveMacro(const char *text);

void mcc_DoMacroReplacement(mcc_LogicalLine_t *line);

#endif /* MCC_MACRO_H */
