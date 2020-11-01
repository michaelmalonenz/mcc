#ifndef _MCC_ENUMS_H_
#define _MCC_ENUMS_H_
#include "parser.h"

mcc_ASTNode_t *parse_enum_specifier(mcc_AST_t *tree);
mcc_ASTNode_t *parse_enumerator_list(mcc_AST_t *tree);
mcc_ASTNode_t *parse_enumerator(mcc_AST_t *tree);

#endif
