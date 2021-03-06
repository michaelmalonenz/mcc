#include "ICE.h"
#include "liberal.h"
#include <stdlib.h>
#include <string.h>

typedef struct ASTNode {
    struct ASTNode *left;
    struct ASTNode *middle;
    struct ASTNode *right;
    const mcc_Token_t *data;
} mcc_ASTNode_t;

struct syntax_tree {
    mcc_ASTNode_t *root;
    mcc_TokenListIterator_t *iterator;
    const mcc_Token_t *currentToken;
    eral_List_t *numbers_to_delete;
};

static mcc_ASTNode_t *parseCommaExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseConditionalExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseLogicalAndExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseLogicalOrExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseBitwiseExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseNotEqualExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseComparisonExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseStrictComparisonExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseExpression(mcc_AST_t *tree);
static mcc_ASTNode_t *parseFactor(mcc_AST_t *tree);
static mcc_ASTNode_t *parseTerm(mcc_AST_t *tree);

#define get_number(x) x->number.number.integer_s

static mcc_Token_t *create_number_token(mcc_AST_t *tree, int number)
{
    mcc_Number_t num = {.number = {.integer_s = number}, .numberType = SIGNED_INT};
    mcc_Token_t *result = mcc_CreateNumberToken(&num, 0, 0, 0);
    eral_ListAppendData(tree->numbers_to_delete, (uintptr_t)result);
    return result;
}

static const mcc_Token_t *evaluate_unary_operands(mcc_AST_t *tree, const mcc_Token_t *operand,
                                                  const mcc_Token_t *operator)
{
    MCC_ASSERT(operator->tokenType == TOK_OPERATOR);

    switch (operator->tokenIndex)
    {
        case OP_NOT:
        {
            return create_number_token(tree, !get_number(operand));
        }
        break;
        default:
        {
         mcc_PrettyErrorToken(operator,
                         "Unimplemented Unary Operator in arithmetic statement: %s\n",
                         operators[operator->tokenIndex]);
        }
    }
    return NULL;
}

static mcc_Token_t *evaluate_operands(mcc_AST_t *tree, const mcc_Token_t *l_operand,
                                      const mcc_Token_t *r_operand, const mcc_Token_t *operator)
{
    MCC_ASSERT(operator->tokenType == TOK_OPERATOR);

    switch (operator->tokenIndex)
    {
        case OP_ADD:
        {
            return create_number_token(tree, get_number(l_operand) + get_number(r_operand));
        }
        break;
        case OP_MINUS:
        {
            return create_number_token(tree, get_number(l_operand) - get_number(r_operand));
        }
        break;
        case OP_DIVIDE:
        {
            return create_number_token(tree, get_number(l_operand) / get_number(r_operand));
        }
        break;
        case OP_MULTIPLY:
        {
            return create_number_token(tree, get_number(l_operand) * get_number(r_operand));
        }
        break;
        case OP_BITWISE_EXCL_OR:
        {
            return create_number_token(tree, get_number(l_operand) ^ get_number(r_operand));
        }
        break;
        case OP_LOGICAL_AND:
        {
            return create_number_token(tree, get_number(l_operand) && get_number(r_operand));
        }
        break;
        case OP_LOGICAL_INCL_OR:
        {
            return create_number_token(tree, get_number(l_operand) || get_number(r_operand));
        }
        break;
        case OP_GREATER_THAN:
        {
            return create_number_token(tree, get_number(l_operand) > get_number(r_operand));
        }
        break;
        case OP_GREATER_EQUAL:
        {
            return create_number_token(tree, get_number(l_operand) >= get_number(r_operand));
        }
        break;
        case OP_LESS_THAN:
        {
            return create_number_token(tree, get_number(l_operand) < get_number(r_operand));
        }
        break;
        case OP_LESS_EQUAL:
        {
            return create_number_token(tree, get_number(l_operand) <= get_number(r_operand));
        }
        break;
        case OP_COMPARE_TO:
        {
            return create_number_token(tree, get_number(l_operand) == get_number(r_operand));
        }
        case OP_NOT_EQUAL:
        {
            return create_number_token(tree, get_number(l_operand) != get_number(r_operand));
        }
        break;
        default:
        {
         mcc_PrettyErrorToken(
                         operator,
                         "Unimplemented Operator in arithmetic statement: %s\n",
                         operators[operator->tokenIndex]);
        }
    }

    return NULL;
}

const mcc_Token_t *evaluatePostOrder(mcc_AST_t *tree, mcc_ASTNode_t *node)
{
    if (node->data->tokenType == TOK_NUMBER || node->data->tokenType == TOK_IDENTIFIER ||
        node->data->tokenType == TOK_CHAR_CONST)
    {
        return node->data;
    }
    else
    {
        const mcc_Token_t *result = NULL;
        const mcc_Token_t *lhs = NULL;
        const mcc_Token_t *rhs = NULL;
#if MCC_DBUG
        if (node->data->tokenType != TOK_OPERATOR)
        {
            mcc_DebugPrintToken(node->data);
        }
#endif
        MCC_ASSERT(node->data->tokenType == TOK_OPERATOR);
        switch (node->data->tokenIndex)
        {
            case OP_NOT:
            {
                lhs = evaluatePostOrder(tree, node->middle);
                result = evaluate_unary_operands(tree, lhs, node->data);
            }
            break;
            case OP_TERNARY_IF:
            {
                lhs = evaluatePostOrder(tree, node->left);
                if (get_number(lhs))
                {
                    result = evaluatePostOrder(tree, node->middle);
                }
                else
                {
                    result = evaluatePostOrder(tree, node->right);
                }
            }
            break;
            case OP_LOGICAL_AND:
            {
                lhs = evaluatePostOrder(tree, node->left);
                if (!get_number(lhs))
                {
                    result = create_number_token(tree, false);
                }
                else
                {
                    rhs = evaluatePostOrder(tree, node->right);
                    result = evaluate_operands(tree, lhs, rhs, node->data);
                }
            }
            break;
            case OP_LOGICAL_INCL_OR:
            {
                lhs = evaluatePostOrder(tree, node->left);
                if (get_number(lhs))
                {
                    result = create_number_token(tree, true);
                }
                else
                {
                    rhs = evaluatePostOrder(tree, node->right);
                    result = evaluate_operands(tree, lhs, rhs, node->data);
                }
            }
            break;
            default:
            {
                if (!node->right || !node->left)
                {
                    mcc_PrettyErrorToken(node->data, "Wrong number of operands for operator %s\n",
                                         node->data->text);
                }
                rhs = evaluatePostOrder(tree, node->right);
                lhs = evaluatePostOrder(tree, node->left);
                result = evaluate_operands(tree, lhs, rhs, node->data);
            }
            break;
        }
        return result;
    }
}

static void delete_ast_node_tree(mcc_ASTNode_t *root)
{
    if (root->left)
    {
        delete_ast_node_tree(root->left);
    }
    if (root->middle)
    {
        delete_ast_node_tree(root->middle);
    }
    if (root->right)
    {
        delete_ast_node_tree(root->right);
    }
    free(root);
}

static mcc_ASTNode_t *ast_node_create(const mcc_Token_t *data)
{
    mcc_ASTNode_t *result = (mcc_ASTNode_t *)malloc(sizeof(mcc_ASTNode_t));
    memset(result, 0, sizeof(mcc_ASTNode_t));
    result->data = data;
    return result;
}

static void GetNonWhitespaceToken(mcc_AST_t *tree)
{
    tree->currentToken = mcc_TokenListGetNonWhitespaceToken(tree->iterator);
}

static mcc_ASTNode_t *parseFactor(mcc_AST_t *tree)
{
    mcc_ASTNode_t *result;
    if (tree->currentToken == NULL)
    {
        mcc_Error("Unexpected end of expression\n");
    }
    if (tree->currentToken->tokenType == TOK_OPERATOR && tree->currentToken->tokenIndex == OP_NOT)
    {
        result = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        result->middle = parseFactor(tree);
        return result;
    }
    else if (tree->currentToken->tokenType == TOK_NUMBER ||
             tree->currentToken->tokenType == TOK_IDENTIFIER ||
             tree->currentToken->tokenType == TOK_CHAR_CONST)
    {
        result = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        return result;
    }
    else if (tree->currentToken->tokenType == TOK_SYMBOL &&
             tree->currentToken->tokenIndex == SYM_OPEN_PAREN)
    {
        GetNonWhitespaceToken(tree);
        result = parseCommaExpression(tree);
        if (tree->currentToken->tokenType != TOK_SYMBOL &&
            tree->currentToken->tokenIndex != SYM_CLOSE_PAREN)
        {
            mcc_PrettyErrorToken(tree->currentToken,
                                 "Unmatched parentheses! Expected ')' but got '%s'\n",
                                 tree->currentToken->text);
        }
        GetNonWhitespaceToken(tree);
        return result;
    }
#if MCC_DBUG
    mcc_DebugPrintToken(tree->currentToken);
#endif
    mcc_PrettyErrorToken(tree->currentToken, "Unknown token in arithmetic expression '%s'\n",
                         tree->currentToken->text);
    return NULL;
}

static mcc_ASTNode_t *parseTerm(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseFactor(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           (tree->currentToken->tokenIndex == OP_MULTIPLY ||
            tree->currentToken->tokenIndex == OP_DIVIDE))
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseFactor(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseTerm(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           (tree->currentToken->tokenIndex == OP_ADD || tree->currentToken->tokenIndex == OP_MINUS))
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseTerm(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseStrictComparisonExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           (tree->currentToken->tokenIndex == OP_LESS_THAN ||
            tree->currentToken->tokenIndex == OP_GREATER_THAN))
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseComparisonExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseStrictComparisonExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           (tree->currentToken->tokenIndex == OP_LESS_EQUAL ||
            tree->currentToken->tokenIndex == OP_GREATER_EQUAL ||
            tree->currentToken->tokenIndex == OP_COMPARE_TO))
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseStrictComparisonExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseNotEqualExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseComparisonExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           tree->currentToken->tokenIndex == OP_NOT_EQUAL)
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseComparisonExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseBitwiseExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseNotEqualExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           (tree->currentToken->tokenIndex == OP_BITWISE_AND ||
            tree->currentToken->tokenIndex == OP_BITWISE_EXCL_OR ||
            tree->currentToken->tokenIndex == OP_BITWISE_INCL_OR))
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseNotEqualExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseLogicalAndExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseBitwiseExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           tree->currentToken->tokenIndex == OP_LOGICAL_AND)
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseBitwiseExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseLogicalOrExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseLogicalAndExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           tree->currentToken->tokenIndex == OP_LOGICAL_INCL_OR)
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->right = parseLogicalAndExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseConditionalExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseLogicalOrExpression(tree);
    while (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
           tree->currentToken->tokenIndex == OP_TERNARY_IF)
    {
        mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        op_node->left = node;
        op_node->middle = parseCommaExpression(tree);
        if (tree->currentToken->tokenType != TOK_OPERATOR &&
            tree->currentToken->tokenIndex != OP_TERNARY_ELSE)
        {
            mcc_PrettyErrorToken(tree->currentToken, "Expected ':' after '?' instead of '%s'\n",
                                 tree->currentToken->text);
        }
        GetNonWhitespaceToken(tree);
        op_node->right = parseConditionalExpression(tree);
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseCommaExpression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parseConditionalExpression(tree);
    if (tree->currentToken && tree->currentToken->tokenType == TOK_OPERATOR &&
        tree->currentToken->tokenIndex == OP_COMMA)
    {
        GetNonWhitespaceToken(tree);
    }
    return node;
}

static mcc_AST_t *create_syntax_tree(mcc_TokenListIterator_t *iter)
{
    mcc_AST_t *result = (mcc_AST_t *)malloc(sizeof(mcc_AST_t));
    result->numbers_to_delete = eral_ListCreate();
    result->root = NULL;
    result->currentToken = NULL;
    result->iterator = iter;
    return result;
}

void mcc_DeleteAST(mcc_AST_t *tree)
{
    eral_ListIterator_t *number_iter = eral_ListGetIterator(tree->numbers_to_delete);
    uintptr_t death_row = eral_ListGetNextData(number_iter);
    while (death_row != NULL_DATA)
    {
        mcc_DeleteToken(death_row);
        death_row = eral_ListGetNextData(number_iter);
    }
    eral_ListDeleteIterator(number_iter);
    eral_ListDelete(tree->numbers_to_delete, NULL);
    delete_ast_node_tree(tree->root);
    free(tree);
}

mcc_Token_t *mcc_ICE_EvaluateAST(mcc_AST_t *tree)
{
    mcc_Token_t *result = mcc_CopyToken(evaluatePostOrder(tree, tree->root));
    mcc_DeleteAST(tree);
    return result;
}

mcc_AST_t *mcc_ParseExpression(mcc_TokenListIterator_t *iter)
{
    mcc_AST_t *tree = create_syntax_tree(iter);
    tree->currentToken = mcc_TokenListPeekCurrentToken(iter);
    tree->root = parseCommaExpression(tree);
    return tree;
}
