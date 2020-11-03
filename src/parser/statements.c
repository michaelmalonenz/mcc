#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "parser_shared.h"

/**
 * <compound-statement> ::= { {{<declaration>}* {<statement>}*}* }
 */
mcc_ASTNode_t *parse_compound_statement(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_SYMBOL, SYM_OPEN_BRACE))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);

        // Need to deal with this properly.
        // mcc_ASTNode_t *decl_node = parse_declaration(tree);
        // mcc_ASTNode_t *statement_node = parse_statement(tree);

        if (!mcc_compare_token(tree->currentToken, TOK_SYMBOL, SYM_CLOSE_BRACE))
        {
            mcc_PrettyErrorToken(tree->currentToken, "Expected '}', got '%s'\n",
                                 tree->currentToken ? tree->currentToken->text : "EOF");
        }
        else
        {
            GetNonWhitespaceToken(tree);
        }
    }
    return node;
}

/**
 * <statement> ::= <labeled-statement>
 *               | <expression-statement>
 *               | <compound-statement>
 *               | <selection-statement>
 *               | <iteration-statement>
 *               | <jump-statement>
 */
mcc_ASTNode_t *parse_statement(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    node = parse_labeled_statement(tree);
    if (node != NULL) return node;

    node = parse_expression_statement(tree);
    if (node != NULL) return node;

    node = parse_compound_statement(tree);
    if (node != NULL) return node;

    node = parse_selection_statement(tree);
    if (node != NULL) return node;

    node = parse_iteration_statement(tree);
    if (node != NULL) return node;

    node = parse_jump_statement(tree);

    return node;
}

/**
 * <labeled-statement> ::= <identifier> : <statement>
 *                       | case <constant-expression> : <statement>
 *                       | default : <statement>
 */
mcc_ASTNode_t *parse_labeled_statement(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <expression-statement> ::= {<expression>}? ;
 */
mcc_ASTNode_t *parse_expression_statement(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_expression(tree);
    if (!mcc_compare_token(tree->currentToken, TOK_SYMBOL, SYM_SEMI_COLON))
    {
        mcc_PrettyErrorToken(tree->currentToken,
                             "Expected a ';' at the end of a statement, got '%s'\n",
                             tree->currentToken ? tree->currentToken->text : "EOF");
    }
    else
    {
        GetNonWhitespaceToken(tree);
    }
    return node;
}

/**
 * <selection-statement> ::= if ( <expression> ) <statement>
 *                         | if ( <expression> ) <statement> else <statement>
 *                         | switch ( <expression> ) <statement>
 */
mcc_ASTNode_t *parse_selection_statement(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <iteration-statement> ::= while ( <expression> ) <statement>
 *                         | do <statement> while ( <expression> ) ;
 *                         | for ( {<expression>}? ; {<expression>}? ; {<expression>}? ) <statement>
 */
mcc_ASTNode_t *parse_iteration_statement(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <jump-statement> ::= goto <identifier> ;
 *                    | continue ;
 *                    | break ;
 *                    | return {<expression>}? ;
 */
mcc_ASTNode_t *parse_jump_statement(mcc_AST_t UNUSED(*tree)) { return NULL; }
