#ifndef _MCC_STATEMENTS_H_
#define _MCC_STATEMENTS_H_
#include "parser.h"

mcc_ASTNode_t *parse_compound_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_labeled_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_expression_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_selection_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_iteration_statement(mcc_AST_t *tree);
mcc_ASTNode_t *parse_jump_statement(mcc_AST_t *tree);

#endif
