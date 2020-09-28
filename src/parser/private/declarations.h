#ifndef _MCC_DECLARATIONS_H_
#define _MCC_DECLARATIONS_H_
#include "parser.h"

mcc_ASTNode_t *parse_translation_unit(mcc_AST_t *tree);
mcc_ASTNode_t *parse_external_declaration(mcc_AST_t *tree);
mcc_ASTNode_t *parse_function_definition(mcc_AST_t *tree);
mcc_ASTNode_t *parse_declaration_specifier(mcc_AST_t *tree);
mcc_ASTNode_t *parse_storage_class_specifier(mcc_AST_t *tree);
mcc_ASTNode_t *parse_type_specifier(mcc_AST_t *tree);

#endif
