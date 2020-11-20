#ifndef _MCC_CONSTANTS_H_
#define _MCC_CONSTANTS_H_
#include "parser.h"

mcc_ASTNode_t *parse_integer_constant(mcc_AST_t *tree);
mcc_ASTNode_t *parse_character_constant(mcc_AST_t *tree);
mcc_ASTNode_t *parse_floating_constant(mcc_AST_t *tree);
mcc_ASTNode_t *parse_enumeration_constant(mcc_AST_t *tree);

#endif
