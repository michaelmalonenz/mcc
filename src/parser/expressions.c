#include "expressions.h"
#include "constants.h"
#include "parser_shared.h"

/**
 * <constant-expression> ::= <conditional-expression>
 */
mcc_ASTNode_t *parse_constant_expression(mcc_AST_t *tree)
{
    return parse_conditional_expression(tree);
}

/**
 * <conditional-expression> ::= <logical-or-expression>
 *                            | <logical-or-expression> ? <expression> : <conditional-expression>
 */
mcc_ASTNode_t *parse_conditional_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_logical_or_expression(tree);
    if (node != NULL && mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_TERNARY_IF))
    {
        node->left = parse_expression(tree);
        if (node->left == NULL)
        {
            mcc_PrettyErrorToken(tree->currentToken, "Expected an expression\n");
        }
        if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_TERNARY_ELSE))
        {
            mcc_PrettyErrorToken(tree->currentToken, "Expected ':' but got '%s'\n",
                                 tree->currentToken ? tree->currentToken->text : "EOF");
        }
        GetNonWhitespaceToken(tree);
        node->right = parse_conditional_expression(tree);
        if (node->right == NULL)
        {
            mcc_PrettyErrorToken(tree->currentToken, "Expected an expression\n");
        }
    }
    return node;
}

/**
 * <logical-or-expression> ::= <logical-and-expression>
 *                           | <logical-or-expression> || <logical-and-expression>
 */
mcc_ASTNode_t *parse_logical_or_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_logical_and_expression(tree);
    if (node == NULL)
    {
        node = parse_logical_or_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_LOGICAL_INCL_OR))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected '||' but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *or_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            or_node->left = node;
            or_node->right = parse_logical_and_expression(tree);
            node = or_node;
        }
    }
    return node;
}

/**
 * <logical-and-expression> ::= <inclusive-or-expression>
 *                            | <logical-and-expression> && <inclusive-or-expression>
 */
mcc_ASTNode_t *parse_logical_and_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_inclusive_or_expression(tree);
    if (node == NULL)
    {
        node = parse_logical_and_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_LOGICAL_AND))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected '&&' but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *and_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            and_node->left = node;
            and_node->right = parse_inclusive_or_expression(tree);
            node = and_node;
        }
    }
    return node;
}

/**
 * <inclusive-or-expression> ::= <exclusive-or-expression>
 *                             | <inclusive-or-expression> | <exclusive-or-expression>
 */
mcc_ASTNode_t *parse_inclusive_or_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_exclusive_or_expression(tree);
    if (node == NULL)
    {
        node = parse_inclusive_or_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_INCL_OR))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected '|' but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *and_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            and_node->left = node;
            and_node->right = parse_exclusive_or_expression(tree);
            node = and_node;
        }
    }
    return node;
}

/**
 * <exclusive-or-expression> ::= <and-expression>
 *                             | <exclusive-or-expression> ^ <and-expression>
 */
mcc_ASTNode_t *parse_exclusive_or_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_and_expression(tree);
    if (node == NULL)
    {
        node = parse_exclusive_or_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_EXCL_OR))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected '^' but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *and_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            and_node->left = node;
            and_node->right = parse_and_expression(tree);
            node = and_node;
        }
    }
    return node;
}

/**
 * <and-expression> ::= <equality-expression>
 *                    | <and-expression> & <equality-expression>
 */
mcc_ASTNode_t *parse_and_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_equality_expression(tree);
    if (node == NULL)
    {
        node = parse_and_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_AND))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected '&' but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *and_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            and_node->left = node;
            and_node->right = parse_equality_expression(tree);
            node = and_node;
        }
    }
    return node;
}

/**
 * <equality-expression> ::= <relational-expression>
 *                         | <equality-expression> == <relational-expression>
 *                         | <equality-expression> != <relational-expression>
 */
mcc_ASTNode_t *parse_equality_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_relational_expression(tree);
    if (node == NULL)
    {
        node = parse_equality_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_COMPARE_TO) &&
                !mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_NOT_EQUAL))
            {
                mcc_PrettyErrorToken(tree->currentToken,
                                     "Expected equality expression, but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            mcc_ASTNode_t *op_node = ast_node_create(tree->currentToken);
            GetNonWhitespaceToken(tree);
            op_node->left = node;
            node = op_node;
            op_node->right = parse_relational_expression(tree);
        }
    }
    return node;
}

/**
 * <relational-expression> ::= <shift-expression>
 *                           | <relational-expression> < <shift-expression>
 *                           | <relational-expression> > <shift-expression>
 *                           | <relational-expression> <= <shift-expression>
 *                           | <relational-expression> >= <shift-expression>
 */
mcc_ASTNode_t *parse_relational_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <shift-expression> ::= <additive-expression>
 *                      | <shift-expression> << <additive-expression>
 *                      | <shift-expression> >> <additive-expression>
 */
mcc_ASTNode_t *parse_shift_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <additive-expression> ::= <multiplicative-expression>
 *                         | <additive-expression> + <multiplicative-expression>
 *                         | <additive-expression> - <multiplicative-expression>
 */
mcc_ASTNode_t *parse_additive_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <multiplicative-expression> ::= <cast-expression>
 *                               | <multiplicative-expression> * <cast-expression>
 *                               | <multiplicative-expression> / <cast-expression>
 *                               | <multiplicative-expression> % <cast-expression>
 */
mcc_ASTNode_t *parse_multiplicative_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <cast-expression> ::= <unary-expression>
 *                     | ( <type-name> ) <cast-expression>
 */
mcc_ASTNode_t *parse_cast_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <unary-expression> ::= <postfix-expression>
 *                      | ++ <unary-expression>
 *                      | -- <unary-expression>
 *                      | <unary-operator> <cast-expression>
 *                      | sizeof <unary-expression>
 *                      | sizeof <type-name>
 */
mcc_ASTNode_t *parse_unary_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <postfix-expression> ::= <primary-expression>
 *                        | <postfix-expression> [ <expression> ]
 *                        | <postfix-expression> ( {<assignment-expression>}* )
 *                        | <postfix-expression> . <identifier>
 *                        | <postfix-expression> -> <identifier>
 *                        | <postfix-expression> ++
 *                        | <postfix-expression> --
 */
mcc_ASTNode_t *parse_postfix_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <primary-expression> ::= <identifier>
 *                        | <constant>
 *                        | <string>
 *                        | ( <expression> )
 */
mcc_ASTNode_t *parse_primary_expression(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <constant> ::= <integer-constant>
 *              | <character-constant>
 *              | <floating-constant>
 *              | <enumeration-constant>
 */
mcc_ASTNode_t *parse_constant(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_integer_constant(tree);
    if (node != NULL) return node;

    node = parse_character_constant(tree);
    if (node != NULL) return node;

    node = parse_floating_constant(tree);
    if (node != NULL) return node;

    node = parse_enumeration_constant(tree);

    return node;
}

/**
 * <expression> ::= <assignment-expression>
 *                | <expression> , <assignment-expression>
 */
mcc_ASTNode_t *parse_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_assignment_expression(tree);
    if (node == NULL)
    {
        node = parse_expression(tree);
        if (node != NULL)
        {
            if (!mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_COMMA))
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected comma ',', but got '%s'\n",
                                     tree->currentToken ? tree->currentToken->text : "EOF");
            }
            GetNonWhitespaceToken(tree);
            node->middle = parse_assignment_expression(tree);
            if (node->middle == NULL)
            {
                mcc_PrettyErrorToken(tree->currentToken, "Expected an expression\n");
            }
        }
    }
    return node;
}

/**
 * <assignment-expression> ::= <conditional-expression>
 *                           | <unary-expression> <assignment-operator> <assignment-expression>
 */
mcc_ASTNode_t *parse_assignment_expression(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_conditional_expression(tree);
    if (node != NULL) return node;

    mcc_ASTNode_t *unary_node = parse_unary_expression(tree);
    node = parse_assignment_operator(tree);
    if (node != NULL)
    {
        node->left = unary_node;
        node->right = parse_assignment_expression(tree);
    }
    else if (unary_node != NULL)
    {
        mcc_PrettyErrorToken(tree->currentToken, "Expected an assignment operator, but got '%s'\n",
                             tree->currentToken->text);
    }

    return node;
}

/**
 * <assignment-operator> ::= =
 *                         | *=
 *                         | /=
 *                         | %=
 *                         | +=
 *                         | -=
 *                         | <<=
 *                         | >>=
 *                         | &=
 *                         | ^=
 *                         | |=
 */
mcc_ASTNode_t *parse_assignment_operator(mcc_AST_t UNUSED(*tree))
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_EQUALS_ASSIGN) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_TIMES_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_DIVIDE_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_MOD_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_PLUS_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_MINUS_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_L_SHIFT_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_R_SHIFT_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_AND_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_EXCL_OR_EQUALS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_BITWISE_INCL_OR_EQUALS))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }

    return node;
}

/**
 * <unary-operator> ::= &
 *                    | *
 *                    | +
 *                    | -
 *                    | ~
 *                    | !
 */
mcc_ASTNode_t *parse_unary_operator(mcc_AST_t UNUSED(*tree))
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_ADDRESS_OF) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_DEREFERENCE) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_ADD) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_MINUS) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_NEGATE) ||
        mcc_compare_token(tree->currentToken, TOK_OPERATOR, OP_NOT))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }

    return node;
}
