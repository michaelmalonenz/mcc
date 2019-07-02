#include "config.h"
#if MCC_USE_AST_ICE
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

static mcc_Stack_t *numbers;

/*
typedef enum token_associativity
{
    TOK_ASSOC_RIGHT,
    TOK_ASSOC_LEFT,
    TOK_ASSOC_UNARY
} token_associativity_t;

static token_associativity_t getOperatorAssociativity(const mcc_Token_t *token)
{
    if (token->tokenType == TOK_OPERATOR &&
        (token->tokenIndex == OP_BITWISE_EXCL_OR || token->tokenIndex == OP_DEREFERENCE ||
         token->tokenIndex == OP_ADDRESS_OF || token->tokenIndex == OP_NOT ||
         token->tokenIndex == OP_NEGATE || token->tokenIndex == OP_SIZEOF ||
         token->tokenIndex == OP_TERNARY_IF || token->tokenIndex == OP_TERNARY_ELSE ||
         token->tokenIndex == OP_TIMES_EQUALS || token->tokenIndex == OP_DIVIDE_EQUALS ||
         token->tokenIndex == OP_MOD_EQUALS || token->tokenIndex == OP_PLUS_EQUALS ||
         token->tokenIndex == OP_MINUS_EQUALS || token->tokenIndex == OP_L_SHIFT_EQUALS ||
         token->tokenIndex == OP_R_SHIFT_EQUALS || token->tokenIndex == OP_BITWISE_AND_EQUALS ||
         token->tokenIndex == OP_BITWISE_EXCL_OR_EQUALS || token->tokenIndex == OP_BITWISE_INCL_OR_EQUALS))
    {
        return TOK_ASSOC_RIGHT;
    }
    return TOK_ASSOC_LEFT;
}
*/

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
         mcc_DebugPrintToken(operator);
         mcc_PrettyError(mcc_ResolveFileNameFromNumber(operator->fileno),
                         operator->lineno,
                         operator->line_index,
                         "Unimplemented Operator in arithmetic statement: %s\n",
                         operators[operator->tokenIndex]);
      }
   }

   return 0;
}

static int getRelativeOperatorPrecedence(MCC_OPERATOR op)
{
    static const int precedents[NUM_OPERATORS] = {
        [OP_MEMBER_OF] = 1,
        [OP_DEREF_MEMBER_OF] = 2,
        [OP_DECREMENT_POST] = 3,
        [OP_INCREMENT_POST] = 3,
        [OP_DEREFERENCE] = 4,
        [OP_ADDRESS_OF] = 4,
        [OP_NOT] = 5,
        [OP_NEGATE] = 6,
        [OP_SIZEOF] = 7,
        [OP_MULTIPLY] = 8,
        [OP_DIVIDE] = 8,
        [OP_MODULO] = 9,
        [OP_ADD] = 10,
        [OP_MINUS] = 10,
        [OP_R_SHIFT] = 11,
        [OP_L_SHIFT] = 11,
        [OP_LESS_THAN] = 12,
        [OP_GREATER_THAN] = 12,
        [OP_LESS_EQUAL] = 13,
        [OP_GREATER_EQUAL] = 13,
        [OP_COMPARE_TO] = 13,
        [OP_NOT_EQUAL] = 14,
        [OP_BITWISE_AND] = 15,
        [OP_BITWISE_EXCL_OR] = 16,
        [OP_BITWISE_INCL_OR] = 17,
        [OP_LOGICAL_AND] = 18,
        [OP_LOGICAL_INCL_OR] = 19,
        [OP_TERNARY_IF] = 20,
        [OP_TERNARY_ELSE] = 20,
        [OP_EQUALS_ASSIGN] = 21,
        [OP_PLUS_EQUALS] = 21,
        [OP_MINUS_EQUALS] = 21,
        [OP_TIMES_EQUALS] = 21,
        [OP_DIVIDE_EQUALS] = 21,
        [OP_MOD_EQUALS] = 21,
        [OP_R_SHIFT_EQUALS] = 21,
        [OP_L_SHIFT_EQUALS] = 21,
        [OP_BITWISE_AND_EQUALS] = 21,
        [OP_BITWISE_EXCL_OR_EQUALS] = 21,
        [OP_BITWISE_INCL_OR_EQUALS] = 21,
        [OP_COMMA] = 22,
        [OP_DECREMENT_PRE] = 23,
        [OP_INCREMENT_PRE] = 23,
    };

    return precedents[op];
}


mcc_ASTNode_t *ast_node_create(mcc_Token_t *data)
{
    mcc_ASTNode_t *result = (mcc_ASTNode_t *)malloc(sizeof(mcc_ASTNode_t));
    memset(result, 0, sizeof(mcc_ASTNode_t));
    result->data = data;
    return result;
}

mcc_Token_t *GetNonWhitespaceToken(mcc_TokenListIterator_t *iter)
{
    mcc_Token_t *token = mcc_GetNextToken(iter);
    if (token == NULL)
        return NULL;
    if (token->tokenType == TOK_WHITESPACE)
    {
        token = mcc_GetNextToken(iter);
    }
    return token;
}

static mcc_Stack_t *parens;
static mcc_ASTNode_t *current;
static bool_t first_paren = FALSE;
void parseExpression(mcc_TokenListIterator_t *iter)
{
    mcc_Token_t *token = GetNonWhitespaceToken(iter);
    if (token == NULL)
        return;
    mcc_ASTNode_t *node = ast_node_create(token);
    if (token->tokenType == TOK_NUMBER)
    {
        mcc_StackPush(numbers, (uintptr_t) node);
    }
    else if (token->tokenType == TOK_OPERATOR)
    {
        if (current != NULL)
        {
            if (first_paren ||
                getRelativeOperatorPrecedence(token->tokenIndex) >
                getRelativeOperatorPrecedence(current->data->tokenIndex))
            {
                current->right = node;
            }
            else
            {
                current->left = node;
            }
        }
        current = node;
        first_paren = FALSE;
    }
    else if (token->tokenType == TOK_SYMBOL)
    {
        if (token->tokenIndex == SYM_OPEN_PAREN)
        {
            free(node);
            node = NULL;
            first_paren = TRUE;
            mcc_StackPush(parens, (uintptr_t) token);
        }
        else if (token->tokenIndex == SYM_CLOSE_PAREN)
        {
            mcc_Token_t *open = (mcc_Token_t *) mcc_StackPop(parens);
            if (open == NULL)
            {
                mcc_PrettyError(mcc_ResolveFileNameFromNumber(token->fileno),
                               token->lineno,
                               token->line_index,
                               "Unmatched Parentheses\n");
            }
        }
    }
    parseExpression(iter);
    if (node && node->data->tokenType == TOK_OPERATOR)
    {
        if (node->right == NULL)
        {
            node->right = (mcc_ASTNode_t *) mcc_StackPop(numbers);
        }
        if (node->left == NULL)
        {
            node->left = (mcc_ASTNode_t *) mcc_StackPop(numbers);
        }
        current = node;
    }
}

int evaluateInOrder(mcc_ASTNode_t *node)
{
    int lhs;
    int rhs;
    if (node->left)
    {
        lhs = evaluateInOrder(node->left);
    }
    if (node->right)
    {
        rhs = evaluateInOrder(node->right);
    }
    if (node->left == NULL && node->right == NULL)
    {
        int result = node->data->number.number.integer_s;
        free(node);
        return result;
    }
    else
    {
        MCC_ASSERT(node->data->tokenType == TOK_OPERATOR);
        return evaluate_operands(lhs, rhs, node->data);
    }
}

int mcc_ICE_EvaluateTokenString(mcc_TokenListIterator_t *iter)
{
    parens = mcc_StackCreate();
    numbers = mcc_StackCreate();
    parseExpression(iter);
    MCC_ASSERT(mcc_StackEmpty(numbers));
    mcc_StackDelete(numbers, NULL);
    mcc_Token_t *open = (mcc_Token_t *) mcc_StackPop(parens);
    if (open != NULL)
    {
        mcc_PrettyError(mcc_ResolveFileNameFromNumber(open->fileno),
                        open->lineno,
                        open->line_index,
                        "Unmatched Parentheses\n");
    }
    mcc_StackDelete(parens, NULL);

    return evaluateInOrder(current);
}
#endif
