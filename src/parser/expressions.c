#include "expressions.h"

/**
 * <constant-expression> ::= <conditional-expression>
 */
mcc_ASTNode_t *parse_constant_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <conditional-expression> ::= <logical-or-expression>
 *                            | <logical-or-expression> ? <expression> : <conditional-expression>
 */
mcc_ASTNode_t *parse_conditional_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <logical-or-expression> ::= <logical-and-expression>
 *                           | <logical-or-expression> || <logical-and-expression>
 */
mcc_ASTNode_t *parse_logical_or_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <logical-and-expression> ::= <inclusive-or-expression>
 *                            | <logical-and-expression> && <inclusive-or-expression>
 */
mcc_ASTNode_t *parse_logical_and_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <inclusive-or-expression> ::= <exclusive-or-expression>
 *                             | <inclusive-or-expression> | <exclusive-or-expression>
 */
mcc_ASTNode_t *parse_inclusive_or_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <exclusive-or-expression> ::= <and-expression>
 *                             | <exclusive-or-expression> ^ <and-expression>
 */
mcc_ASTNode_t *parse_exclusive_or_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <and-expression> ::= <equality-expression>
 *                    | <and-expression> & <equality-expression>
 */
mcc_ASTNode_t *parse_and_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <equality-expression> ::= <relational-expression>
 *                         | <equality-expression> == <relational-expression>
 *                         | <equality-expression> != <relational-expression>
 */
mcc_ASTNode_t *parse_equality_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <relational-expression> ::= <shift-expression>
 *                           | <relational-expression> < <shift-expression>
 *                           | <relational-expression> > <shift-expression>
 *                           | <relational-expression> <= <shift-expression>
 *                           | <relational-expression> >= <shift-expression>
 */
mcc_ASTNode_t *parse_relational_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <shift-expression> ::= <additive-expression>
 *                      | <shift-expression> << <additive-expression>
 *                      | <shift-expression> >> <additive-expression>
 */
mcc_ASTNode_t *parse_shift_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <additive-expression> ::= <multiplicative-expression>
 *                         | <additive-expression> + <multiplicative-expression>
 *                         | <additive-expression> - <multiplicative-expression>
 */
mcc_ASTNode_t *parse_additive_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <multiplicative-expression> ::= <cast-expression>
 *                               | <multiplicative-expression> * <cast-expression>
 *                               | <multiplicative-expression> / <cast-expression>
 *                               | <multiplicative-expression> % <cast-expression>
 */
mcc_ASTNode_t *parse_multiplicative_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <cast-expression> ::= <unary-expression>
 *                     | ( <type-name> ) <cast-expression>
 */
mcc_ASTNode_t *parse_cast_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <unary-expression> ::= <postfix-expression>
 *                      | ++ <unary-expression>
 *                      | -- <unary-expression>
 *                      | <unary-operator> <cast-expression>
 *                      | sizeof <unary-expression>
 *                      | sizeof <type-name>
 */
mcc_ASTNode_t *parse_unary_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <postfix-expression> ::= <primary-expression>
 *                        | <postfix-expression> [ <expression> ]
 *                        | <postfix-expression> ( {<assignment-expression>}* )
 *                        | <postfix-expression> . <identifier>
 *                        | <postfix-expression> -> <identifier>
 *                        | <postfix-expression> ++
 *                        | <postfix-expression> --
 */
mcc_ASTNode_t *parse_postfix_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <primary-expression> ::= <identifier>
 *                        | <constant>
 *                        | <string>
 *                        | ( <expression> )
 */
mcc_ASTNode_t *parse_primary_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <constant> ::= <integer-constant>
 *              | <character-constant>
 *              | <floating-constant>
 *              | <enumeration-constant>
 */
mcc_ASTNode_t *parse_constant(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <expression> ::= <assignment-expression>
 *                | <expression> , <assignment-expression>
 */
mcc_ASTNode_t *parse_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <assignment-expression> ::= <conditional-expression>
 *                           | <unary-expression> <assignment-operator> <assignment-expression>
 */
mcc_ASTNode_t *parse_assignment_expression(mcc_AST_t UNUSED(*tree))
{
    return NULL;
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
    return NULL;
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
    return NULL;
}
