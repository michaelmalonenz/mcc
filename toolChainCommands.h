#ifndef _MCC_TOOL_CHAIN_COMMANDS_H_
#define _MCC_TOOL_CHAIN_COMMANDS_H_

#include "mcc.h"
#include "tokenList.h"

void mcc_PreprocessCurrentTokens(void);
void mcc_TokeniseFile(const char *inFilename, mcc_TokenListIterator_t *iter);

#endif /* _MCC_TOOL_CHAIN_COMMANDS_H_ */
