#include <string.h>
#include "parser.h"
#include "parser_shared.h"
#include "declarations.h"

bool mcc_compare_token(const mcc_Token_t *token, TOKEN_TYPE type, int index)
{
    return (token && token->tokenType == type && token->tokenIndex == index);
}

void GetNonWhitespaceToken(mcc_AST_t *tree)
{
    tree->currentToken = mcc_TokenListGetNonWhitespaceToken(tree->iterator);
}

static void delete_ast_node_tree(mcc_ASTNode_t *root)
{
    if (root == NULL)
        return;
    delete_ast_node_tree(root->left);
    delete_ast_node_tree(root->middle);
    delete_ast_node_tree(root->right);
    free(root);
}

mcc_ASTNode_t *ast_node_create(const mcc_Token_t *data)
{
    mcc_ASTNode_t *result = (mcc_ASTNode_t *)malloc(sizeof(mcc_ASTNode_t));
    memset(result, 0, sizeof(mcc_ASTNode_t));
    result->data = data;
    return result;
}

static mcc_AST_t *create_syntax_tree(mcc_TokenListIterator_t *iter)
{
    mcc_AST_t *result = (mcc_AST_t *)malloc(sizeof(mcc_AST_t));
    result->root = NULL;
    result->currentToken = NULL;
    result->iterator = iter;
    return result;
}

void mcc_delete_ast(mcc_AST_t *tree)
{
    delete_ast_node_tree(tree->root);
    free(tree);
}

mcc_AST_t *mcc_parse_tokens(mcc_TokenList_t *tokens)
{
    mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(tokens);
    mcc_AST_t *tree = create_syntax_tree(iter);
    tree->currentToken = mcc_TokenListPeekCurrentToken(iter);
    tree->root = parse_translation_unit(tree);
    return tree;
}