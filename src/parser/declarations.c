#include "declarations.h"
#include "parser_shared.h"

mcc_ASTNode_t *parse_translation_unit(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_external_declaration(tree);
    return node;
}

mcc_ASTNode_t *parse_external_declaration(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_function_definition(tree);
    while (node != NULL)
    {
        mcc_ASTNode_t *func_node = parse_function_definition(tree);
        node->middle = func_node;
        node = func_node;
    }
    return node;
}

mcc_ASTNode_t *parse_function_definition(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_declaration_specifier(tree);
    return node;
}

mcc_ASTNode_t *parse_declarator(mcc_AST_t *tree)
{
    return parse_direct_declarator(tree);
}

mcc_ASTNode_t *parse_direct_declarator(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = NULL;
    if (mcc_compare_token(tree->currentToken, TOK_IDENTIFIER, TOK_UNSET_INDEX))
    {
        node = ast_node_create(tree->currentToken);
    }
    return node;
}

mcc_ASTNode_t *parse_declaration_specifier(mcc_AST_t *tree)
{
    mcc_ASTNode_t *node = parse_storage_class_specifier(tree);
    mcc_ASTNode_t *type_node = parse_type_specifier(tree);
    while (type_node != NULL)
    {
        node->middle = type_node;
        node = type_node;
        type_node = parse_type_specifier(tree);
    }
    return node;
}

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
    else
    {
        mcc_PrettyErrorToken(tree->currentToken, "Expected type specifier");
    }
    // | <struct-or-union-specifier>
    // | <enum-specifier>
    // | <typedef-name>
    return result;
}
