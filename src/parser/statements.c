#include "statements.h"

/**
 * <compound-statement> ::= { {<declaration>}* {<statement>}* }
 */
mcc_ASTNode_t *parse_compound_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <statement> ::= <labeled-statement>
 *               | <expression-statement>
 *               | <compound-statement>
 *               | <selection-statement>
 *               | <iteration-statement>
 *               | <jump-statement>
 */
mcc_ASTNode_t *parse_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <labeled-statement> ::= <identifier> : <statement>
 *                       | case <constant-expression> : <statement>
 *                       | default : <statement>
 */
mcc_ASTNode_t *parse_labeled_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <expression-statement> ::= {<expression>}? ;
 */
mcc_ASTNode_t *parse_expression_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <selection-statement> ::= if ( <expression> ) <statement>
 *                         | if ( <expression> ) <statement> else <statement>
 *                         | switch ( <expression> ) <statement>
 */
mcc_ASTNode_t *parse_selection_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <iteration-statement> ::= while ( <expression> ) <statement>
 *                         | do <statement> while ( <expression> ) ;
 *                         | for ( {<expression>}? ; {<expression>}? ; {<expression>}? ) <statement>
 */
mcc_ASTNode_t *parse_iteration_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <jump-statement> ::= goto <identifier> ;
 *                    | continue ;
 *                    | break ;
 *                    | return {<expression>}? ;
 */
mcc_ASTNode_t *parse_jump_statement(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}
