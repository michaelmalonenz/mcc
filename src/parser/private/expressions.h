#ifndef _MCC_EXPRESSIONS_H_
#define _MCC_EXPRESSIONS_H_
#include "parser.h"

mcc_ASTNode_t *parse_constant_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_conditional_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_logical_or_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_logical_and_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_inclusive_or_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_exclusive_or_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_and_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_equality_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_relational_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_shift_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_additive_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_multiplicative_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_cast_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_unary_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_postfix_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_primary_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_expression(mcc_AST_t *tree);
mcc_ASTNode_t *parse_assignment_expression(mcc_AST_t *tree);

#endif
