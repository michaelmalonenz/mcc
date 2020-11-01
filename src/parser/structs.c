#include "structs.h"


/**
 * <struct-or-union-specifier> ::= <struct-or-union> <identifier> { {<struct-declaration>}+ }
 *                               | <struct-or-union> { {<struct-declaration>}+ }
 *                               | <struct-or-union> <identifier>
 */
mcc_ASTNode_t *parse_struct_or_union_specifier(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <struct-or-union> ::= struct
 *                     | union
 */
mcc_ASTNode_t *parse_struct_or_union(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <struct-declaration> ::= {<specifier-qualifier>}* <struct-declarator-list>
 */
mcc_ASTNode_t *parse_struct_declaration(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <specifier-qualifier> ::= <type-specifier>
 *                         | <type-qualifier>
 */
mcc_ASTNode_t *parse_specifier_qualifier(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <struct-declarator-list> ::= <struct-declarator>
 *                            | <struct-declarator-list> , <struct-declarator>
 */
mcc_ASTNode_t *parse_struct_declarator_list(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}

/**
 * <struct-declarator> ::= <declarator>
 *                       | <declarator> : <constant-expression>
 *                       | : <constant-expression>
 */
mcc_ASTNode_t *parse_struct_declarator(mcc_AST_t UNUSED(*tree))
{
    return NULL;
}
