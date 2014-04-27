#ifndef _MCC_SYMBOL_TABLE_H_
#define _MCC_SYMBOL_TABLE_H_
#include "mcc.h"
#include "tokens.h"

struct symbol_table;
typedef struct symbol_table mcc_SymbolTable_t;

bool_t mcc_IsFunction(const mcc_Token_t *token);


#endif /* _MCC_SYMBOL_TABLE_H_ */

