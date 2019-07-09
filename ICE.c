#include <stdlib.h>
#include <string.h>
#include "ICE.h"
#include "stack.h"

typedef struct ASTNode
{
    struct ASTNode *left;
    struct ASTNode *middle;
    struct ASTNode *right;
    mcc_Token_t *data;
} mcc_ASTNode_t;

static mcc_ASTNode_t *parseTernaryExpression(void);
static mcc_ASTNode_t *parseLogicalAndExpression(void);
static mcc_ASTNode_t *parseLogicalOrExpression(void);
static mcc_ASTNode_t *parseBitwiseExpression(void);
static mcc_ASTNode_t *parseNotEqualExpression(void);
static mcc_ASTNode_t *parseComparisonExpression(void);
static mcc_ASTNode_t *parseStrictComparisonExpression(void);
static mcc_ASTNode_t *parseExpression(void);
static mcc_ASTNode_t *parseFactor(void);
static mcc_ASTNode_t *parseTerm(void);

static mcc_TokenListIterator_t *iterator;
static mcc_Token_t *currentToken;

#define ICE_Error(...) \
    mcc_PrettyError(mcc_ResolveFileNameFromNumber(currentToken->fileno), \
        currentToken->lineno, \
        currentToken->line_index, \
        __VA_ARGS__)

static int evaluate_unary_operands(int operand, mcc_Token_t *operator)
{
   MCC_ASSERT(operator->tokenType == TOK_OPERATOR);

   switch (operator->tokenIndex)
   {
      case OP_NOT:
      {
         return !operand;
      }
      break;
      default:
      {
         mcc_PrettyError(mcc_ResolveFileNameFromNumber(operator->fileno),
                         operator->lineno,
                         operator->line_index,
                         "Unimplemented Unary Operator in arithmetic statement: %s\n",
                         operators[operator->tokenIndex]);
      }
   }
   return 0;
}

static int evaluate_operands(int l_operand,
                             int r_operand,
                             mcc_Token_t *operator)
{
   MCC_ASSERT(operator->tokenType == TOK_OPERATOR);

   switch (operator->tokenIndex)
   {
      case OP_ADD:
      {
        return l_operand + r_operand;
      }
      break;
      case OP_MINUS:
      {
        return l_operand - r_operand;
      }
      break;
      case OP_DIVIDE:
      {
          return l_operand / r_operand;
      }
      break;
      case OP_DEREFERENCE: //this is a total lie!
      case OP_MULTIPLY:
      {
          return l_operand * r_operand;
      }
      break;
      case OP_BITWISE_EXCL_OR:
      {
          return l_operand ^ r_operand;
      }
      break;
      case OP_LOGICAL_AND:
      {
          return l_operand && r_operand;
      }
      break;
      case OP_LOGICAL_INCL_OR:
      {
          return l_operand || r_operand;
      }
      break;
      case OP_GREATER_THAN:
      {
          return l_operand > r_operand;
      }
      break;
      case OP_GREATER_EQUAL:
      {
          return l_operand >= r_operand;
      }
      break;
      case OP_LESS_THAN:
      {
          return l_operand < r_operand;
      }
      break;
      case OP_LESS_EQUAL:
      {
          return l_operand <= r_operand;
      }
      break;
      case OP_COMPARE_TO:
      {
          return l_operand == r_operand;
      }
      break;
      default:
      {
         mcc_PrettyError(mcc_ResolveFileNameFromNumber(operator->fileno),
                         operator->lineno,
                         operator->line_index,
                         "Unimplemented Operator in arithmetic statement: %s\n",
                         operators[operator->tokenIndex]);
      }
   }

   return 0;
}

int evaluatePostOrder(mcc_ASTNode_t *node)
{
    if (node->data->tokenType == TOK_NUMBER)
    {
        return node->data->number.number.integer_s;
    }
    else
    {
        MCC_ASSERT(node->data->tokenType == TOK_OPERATOR);
        switch(node->data->tokenIndex)
        {
            case OP_NOT:
            {
                int operand = evaluatePostOrder(node->middle);
                return evaluate_unary_operands(operand, node->data);
            }
            break;
            case OP_TERNARY_IF:
            {
                int operand = evaluatePostOrder(node->left);
                if (operand)
                {
                    return evaluatePostOrder(node->middle);
                }
                else
                {
                    return evaluatePostOrder(node->right);
                }
            }
            break;
            case OP_LOGICAL_AND:
            {
                int lhs = evaluatePostOrder(node->left);
                if (!lhs)
                {
                    return 0;
                }
                return evaluate_operands(lhs, evaluatePostOrder(node->right), node->data);
            }
            break;
            case OP_LOGICAL_INCL_OR:
            {
                int lhs = evaluatePostOrder(node->left);
                if (lhs)
                {
                    return 1;
                }
                return evaluate_operands(lhs, evaluatePostOrder(node->right), node->data);
            }
            default:
            {
                int lhs;
                int rhs;
                if (node->right)
                {
                    rhs = evaluatePostOrder(node->right);
                }
                if (node->left)
                {
                    lhs = evaluatePostOrder(node->left);
                }
                return evaluate_operands(lhs, rhs, node->data);
            }
        }
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

static mcc_ASTNode_t *ast_node_create(mcc_Token_t *data)
{
    mcc_ASTNode_t *result = (mcc_ASTNode_t *)malloc(sizeof(mcc_ASTNode_t));
    memset(result, 0, sizeof(mcc_ASTNode_t));
    result->data = data;
    return result;
}

static void GetNonWhitespaceToken(void)
{
    mcc_Token_t *token = mcc_GetNextToken(iterator);
    if (token == NULL) {
        currentToken = NULL;
        return;
    }
    if (token->tokenType == TOK_WHITESPACE)
    {
        token = mcc_GetNextToken(iterator);
    }
    currentToken = token;
}

static mcc_ASTNode_t *parseFactor(void)
{
    mcc_ASTNode_t *result;
    if (currentToken == NULL)
    {
        mcc_Error("Unexpected end of expression\n");
    }
    if (currentToken->tokenType == TOK_OPERATOR &&
        currentToken->tokenIndex == OP_NOT)
    {
        result = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        result->middle = parseFactor();
        return result;
    }
    else if (currentToken->tokenType == TOK_NUMBER)
    {
        result = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        return result;
    }
    else if (currentToken->tokenType == TOK_SYMBOL &&
             currentToken->tokenIndex == SYM_OPEN_PAREN)
    {
        GetNonWhitespaceToken();
        result = parseTernaryExpression();
        if (currentToken->tokenType != TOK_SYMBOL &&
            currentToken->tokenIndex != SYM_CLOSE_PAREN)
        {
            ICE_Error(
                "Unmatched parentheses! Expected ')' but got '%s'\n",
                currentToken->text);
        }
        GetNonWhitespaceToken();
        return result;
    }
    ICE_Error(
        "Unknown token in arithmetic expression '%s'\n",
        currentToken->text);
    return NULL;
}

static mcc_ASTNode_t *parseTerm(void)
{
    mcc_ASTNode_t *node = parseFactor();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        (currentToken->tokenIndex == OP_MULTIPLY ||
         currentToken->tokenIndex == OP_DIVIDE))
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseFactor();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseExpression()
{
    mcc_ASTNode_t *node = parseTerm();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        (currentToken->tokenIndex == OP_ADD ||
         currentToken->tokenIndex == OP_MINUS))
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseTerm();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseStrictComparisonExpression(void)
{
    mcc_ASTNode_t *node = parseExpression();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        (currentToken->tokenIndex == OP_LESS_THAN ||
         currentToken->tokenIndex == OP_GREATER_THAN))
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseComparisonExpression(void)
{
    mcc_ASTNode_t *node = parseStrictComparisonExpression();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        (currentToken->tokenIndex == OP_LESS_EQUAL ||
         currentToken->tokenIndex == OP_GREATER_EQUAL ||
         currentToken->tokenIndex == OP_COMPARE_TO))
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseStrictComparisonExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseNotEqualExpression(void)
{
    mcc_ASTNode_t *node = parseComparisonExpression();
    while (currentToken && 
           currentToken->tokenType == TOK_OPERATOR &&
           currentToken->tokenIndex == OP_NOT_EQUAL)
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseComparisonExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseBitwiseExpression(void)
{
    mcc_ASTNode_t *node = parseNotEqualExpression();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        (currentToken->tokenIndex == OP_BITWISE_AND ||
         currentToken->tokenIndex == OP_BITWISE_EXCL_OR ||
         currentToken->tokenIndex == OP_BITWISE_INCL_OR))
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseNotEqualExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseLogicalAndExpression(void)
{
    mcc_ASTNode_t *node = parseBitwiseExpression();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        currentToken->tokenIndex == OP_LOGICAL_AND)
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseBitwiseExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseLogicalOrExpression(void)
{
    mcc_ASTNode_t *node = parseLogicalAndExpression();
    while (currentToken &&
        currentToken->tokenType == TOK_OPERATOR &&
        currentToken->tokenIndex == OP_LOGICAL_INCL_OR)
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->right = parseLogicalAndExpression();
        node = op_node;
    }

    return node;
}

static mcc_ASTNode_t *parseTernaryExpression(void)
{
    mcc_ASTNode_t *node = parseLogicalOrExpression();
    while (currentToken &&
           currentToken->tokenType == TOK_OPERATOR &&
           currentToken->tokenIndex == OP_TERNARY_IF)
    {
        mcc_ASTNode_t *op_node = ast_node_create(currentToken);
        GetNonWhitespaceToken();
        op_node->left = node;
        op_node->middle = parseLogicalOrExpression();
        if (currentToken->tokenType != TOK_OPERATOR &&
            currentToken->tokenIndex != OP_TERNARY_ELSE)
        {
            ICE_Error("Expected ':' after '?' instead of '%s'\n", currentToken->text);
        }
        GetNonWhitespaceToken();
        op_node->right = parseLogicalOrExpression();
        node = op_node;
    }

    return node;
}

int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter)
{
    iterator = iter;
    GetNonWhitespaceToken();
    mcc_ASTNode_t *root = parseTernaryExpression();
    int result = evaluatePostOrder(root);
    delete_ast_node_tree(root);
    return result;
}
