#ifndef _MCC_PARSER_H_
#define _MCC_PARSER_H_

#include "mcc.h"
#include "tokenList.h"
#include "list.h"

typedef struct ASTNode
{
    struct ASTNode *left;
    struct ASTNode *middle;
    struct ASTNode *right;
    const mcc_Token_t *data;
} mcc_ASTNode_t;

typedef struct
{
    mcc_ASTNode_t *root;
    mcc_TokenListIterator_t *iterator;
    const mcc_Token_t *currentToken;
} mcc_AST_t;

/**
 * Parse.  Main entry into the world of parsing the tokens
 * to output an AST.
 */
mcc_AST_t *mcc_parse_tokens(mcc_TokenList_t *tokens);

#endif
