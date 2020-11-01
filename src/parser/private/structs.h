#ifndef _MCC_STRUCTS_H_
#define _MCC_STRUCTS_H_

#include "parser.h"

mcc_ASTNode_t *parse_struct_or_union_specifier(mcc_AST_t *tree);
mcc_ASTNode_t *parse_struct_or_union(mcc_AST_t *tree);
mcc_ASTNode_t *parse_struct_declaration(mcc_AST_t *tree);
mcc_ASTNode_t *parse_specifier_qualifier(mcc_AST_t *tree);
mcc_ASTNode_t *parse_struct_declarator_list(mcc_AST_t *tree);
mcc_ASTNode_t *parse_struct_declarator(mcc_AST_t *tree);

#endif
