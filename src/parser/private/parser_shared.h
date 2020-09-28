#ifndef _MCC_PARSER_SHARED_H_
#define _MCC_PARSER_SHARED_H_
#include <stdbool.h>

mcc_ASTNode_t *ast_node_create(const mcc_Token_t *data);

bool mcc_compare_token(const mcc_Token_t *token, TOKEN_TYPE type, int index);

void GetNonWhitespaceToken(mcc_AST_t *tree);

#endif