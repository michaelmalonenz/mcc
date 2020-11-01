#include "declarations.h"
#include "parser_shared.h"
#include "statements.h"
#include "structs.h"
#include "enums.h"

/**
 * <translation-unit> ::= {<external-declaration>}*
 */
mcc_ASTNode_t *parse_translation_unit(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_external_declaration(tree);
    while (tree->currentToken)
    {
        node->middle = parse_external_declaration(tree);
        node = node->middle;
    }
    return node;
}

/**
 * <external-declaration> ::= <function-definition>
 *                          | <declaration>
 */
mcc_ASTNode_t *parse_external_declaration(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_function_definition(tree);
    if (node == NULL)
    {
        node = parse_declaration(tree);
    }
    return node;
}

/**
 *  <declaration> ::=  {<declaration-specifier>}+ {<init-declarator>}* ;
 */
mcc_ASTNode_t *parse_declaration(mcc_AST_t *tree)
{
    mcc_ASTNode_t *specifier_node = parse_declaration_specifier(tree);
    if (specifier_node == NULL)
        mcc_PrettyErrorToken(
            tree->currentToken,
            "Expected a declaration, got '%s'",
            tree->currentToken ? tree->currentToken->text : "EOF");
    return specifier_node;
}

/**
 * <function-definition> ::= {<declaration-specifier>}* <declarator> {<declaration>}* <compound-statement>
 */
mcc_ASTNode_t *parse_function_definition(mcc_AST_t *tree)
{
    mcc_ASTNode_t *specifier_node = parse_declaration_specifier(tree);
    while (specifier_node != NULL)
    {
        specifier_node->left = parse_declaration_specifier(tree);
        if (specifier_node->left == NULL)
            break;
        specifier_node = specifier_node->left;
    }
    mcc_ASTNode_t *declarator_node = parse_declarator(tree);
    if (declarator_node != NULL)
    {
        declarator_node->left = specifier_node;
        mcc_ASTNode_t *declaration_node = parse_declaration(tree);
        while (declaration_node != NULL)
        {
            declaration_node->middle = parse_declaration(tree);
            if (declaration_node->middle == NULL)
                break;
            declaration_node = declaration_node->middle;
        }
        declarator_node->middle = declaration_node;
        declarator_node->right = parse_compound_statement(tree);
    }
    return declarator_node;
}

/**
 * <declarator> ::= {<pointer>}? <direct-declarator>
 */
mcc_ASTNode_t *parse_declarator(mcc_AST_t *tree)
{
    return parse_direct_declarator(tree);
}

/**
 * <direct-declarator> ::= <identifier>
 *                       | ( <declarator> )
 *                       | <direct-declarator> [ {<constant-expression>}? ]
 *                       | <direct-declarator> ( <parameter-type-list> )
 *                       | <direct-declarator> ( {<identifier>}* )
 */
mcc_ASTNode_t *parse_direct_declarator(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }
    else if (mcc_compare_token(tree->currentToken, TOK_SYMBOL, SYM_OPEN_PAREN))
    {
        GetNonWhitespaceToken(tree);
        node = parse_declarator(tree);
        if (mcc_compare_token(tree->currentToken, TOK_SYMBOL, SYM_CLOSE_PAREN))
        {
            GetNonWhitespaceToken(tree);
        }
        else
        {
            mcc_PrettyErrorToken(
                tree->currentToken,
                "Expected closing parenthesis ')', got '%s'",
                tree->currentToken ? tree->currentToken->text : "EOF");
        }
    }
    return node;
}

/**
 * <declaration-specifier> ::= <storage-class-specifier>
 *                           | <type-specifier>
 *                           | <type-qualifier>
 */
mcc_ASTNode_t *parse_declaration_specifier(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_storage_class_specifier(tree);
    if (node == NULL)
    {
        node = parse_type_specifier(tree);
    }
    if (node == NULL)
    {
        node = parse_type_qualifier(tree);
    }
    return node;
}

/**
 * <type-qualifier> ::= const
 *                    | volatile
 */
mcc_ASTNode_t *parse_type_qualifier(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_CONST) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_VOLATILE))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }

    return node;
}

/**
 * <storage-class-specifier> ::= auto
 *                             | register
 *                             | static
 *                             | extern
 *                             | typedef
 */
mcc_ASTNode_t *parse_storage_class_specifier(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_AUTO) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_REGISTER) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_STATIC) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_EXTERN) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_TYPEDEF))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }

    return node;
}

/**
 * <type-specifier> ::= void
 *                    | char
 *                    | short
 *                    | int
 *                    | long
 *                    | float
 *                    | double
 *                    | signed
 *                    | unsigned
 *                    | <struct-or-union-specifier>
 *                    | <enum-specifier>
 *                    | <typedef-name>
 */
mcc_ASTNode_t *parse_type_specifier(mcc_AST_t *tree)
{
    mcc_ASTNode_t* result = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_VOID) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_CHAR) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_SHORT) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_INT) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_LONG) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_FLOAT) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_DOUBLE) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_SIGNED) ||
        mcc_compare_token(tree->currentToken, TOK_KEYWORD, KEY_UNSIGNED))
    {
        result = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
        return result;
    }
    result = parse_struct_or_union_specifier(tree);
    if (result != NULL)
        return result;

    result = parse_enum_specifier(tree);
    if (result != NULL)
        return result;

    result = parse_typedef_name(tree);

    return result;
}

/**
 * <typedef-name> ::= <identifier>
 */
mcc_ASTNode_t *parse_typedef_name(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX))
    {
        node = ast_node_create(tree->currentToken);
        GetNonWhitespaceToken(tree);
    }
    return node;
}
