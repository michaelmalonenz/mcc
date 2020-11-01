#include "enums.h"

/**
 * <enum-specifier> ::= enum <identifier> { <enumerator-list> }
 *                    | enum { <enumerator-list> }
 *                    | enum <identifier>
 */
mcc_ASTNode_t *parse_enum_specifier(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <enumerator-list> ::= <enumerator>
 *                     | <enumerator-list> , <enumerator>
 */
mcc_ASTNode_t *parse_enumerator_list(mcc_AST_t UNUSED(*tree)) { return NULL; }

/**
 * <enumerator> ::= <identifier>
 *                | <identifier> = <constant-expression>
 */
mcc_ASTNode_t *parse_enumerator(mcc_AST_t UNUSED(*tree)) { return NULL; }
