#include "macro_expansion.h"
#include <string.h>
#include <stdbool.h>

mcc_TokenList_t *expandMacroTokens(mcc_Macro_t *macro)
{
    mcc_TokenList_t *result = mcc_TokenListCreate();
    mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(macro->tokens);
    mcc_Token_t *macroToken = mcc_GetNextToken(iter);
    while (macroToken != NULL)
    {
        mcc_TokenListAppend(result, mcc_CopyToken(macroToken));
        macroToken = mcc_GetNextToken(iter);
    }
    mcc_TokenListDeleteIterator(iter);
    return result;
}

static mcc_TokenList_t *getReplacedMacroFunctionTokens(mcc_Macro_t *macro, eral_List_t *parameters)
{
    eral_ListIterator_t *parametersIter = eral_ListGetIterator(parameters);
    mcc_MacroParameter_t *param = (mcc_MacroParameter_t *)eral_ListGetNextData(parametersIter);
    mcc_TokenList_t *working_tokens = mcc_TokenListDeepCopy(macro->tokens);
    if (param == NULL)
    {
        mcc_TokenListDeleteIterator(parametersIter);
        return working_tokens;
    }
    mcc_TokenList_t *result = mcc_TokenListCreate();
    while (param != NULL)
    {
        mcc_TokenListIterator_t *tokensIter = mcc_TokenListGetIterator(working_tokens);
        mcc_Token_t *functionToken = mcc_GetNextToken(tokensIter);
        while (functionToken != NULL)
        {
            bool token_handled = false;
            if (param->argument == NULL && functionToken->tokenType == TOK_IDENTIFIER &&
                strcmp(functionToken->text, "__VA_ARGS__") == 0)
            {
                eral_ListIterator_t *iter_copy = eral_ListCopyIterator(parametersIter);
                mcc_MacroParameter_t *currentParam = param;
                while (currentParam != NULL)
                {
                    mcc_TokenListConcatenate(result,
                                             mcc_TokenListDeepCopy(currentParam->parameterTokens));
                    currentParam = (mcc_MacroParameter_t *)eral_ListGetNextData(iter_copy);
                }
                eral_ListDeleteIterator(iter_copy);
                token_handled = true;
            }
            else if (functionToken->tokenType == TOK_IDENTIFIER && param->argument != NULL &&
                     strcmp(functionToken->text, param->argument->text) == 0)
            {
                mcc_TokenList_t *paramTokens = mcc_TokenListDeepCopy(param->parameterTokens);
                mcc_TokenListConcatenate(result, paramTokens);
                token_handled = true;
            }
            else if (functionToken->tokenType == TOK_IDENTIFIER)
            {
                mcc_Macro_t *func = mcc_ResolveMacro(functionToken->text);
                if (func && func->is_function)
                {
                    printf("Found function '%s' inside '%s'\n", func->text, macro->text);
                    // mcc_TokenList_t *funcTokens =
                    // handleRecursiveMacroFunction(tokensIter,
                    // func);
                    token_handled = true;
                }
            }
            if (!token_handled)
            {
                mcc_TokenListAppend(result, mcc_CopyToken(functionToken));
            }
            functionToken = mcc_GetNextToken(tokensIter);
        }
        mcc_TokenListDeleteIterator(tokensIter);
        param = (mcc_MacroParameter_t *)eral_ListGetNextData(parametersIter);
        mcc_TokenListDelete(working_tokens);
        if (param != NULL)
        {
            working_tokens = result;
            result = mcc_TokenListCreate();
        }
    }
    mcc_TokenListDeleteIterator(parametersIter);
    return result;
}

static void rescanMacroFunctionForActions(mcc_TokenList_t *tokens)
{
    mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator(tokens);
    mcc_Token_t *token = mcc_GetNextToken(iter);
    while (token != NULL)
    {
        if (token->tokenType == TOK_PP_DIRECTIVE && token->tokenIndex == PP_JOIN)
        {
            mcc_DeleteToken((uintptr_t)mcc_TokenListRemoveCurrent(iter));
            mcc_Token_t *lhs = mcc_TokenListRemoveCurrent(iter);
            if (lhs->tokenType == TOK_WHITESPACE)
            {
                mcc_DeleteToken((uintptr_t)lhs);
                lhs = mcc_TokenListRemoveCurrent(iter);
            }

            (void)mcc_GetNextToken(iter);
            mcc_Token_t *rhs = mcc_TokenListRemoveCurrent(iter);
            if (rhs->tokenType == TOK_WHITESPACE)
            {
                mcc_DeleteToken((uintptr_t)rhs);
                (void)mcc_GetNextToken(iter);
                rhs = mcc_TokenListRemoveCurrent(iter);
            }
            eral_StringBuffer_t *sbuffer = eral_CreateStringBuffer();
            eral_StringBufferAppendString(sbuffer, lhs->text);
            eral_StringBufferAppendString(sbuffer, rhs->text);
            mcc_Token_t *tok = mcc_CreateToken(
                eral_StringBufferGetString(sbuffer), eral_GetStringBufferLength(sbuffer),
                TOK_IDENTIFIER, TOK_UNSET_INDEX, lhs->line_index + 1, lhs->lineno, lhs->fileno);
            mcc_InsertToken(tok, iter);
            eral_DeleteStringBuffer(sbuffer);
            mcc_DeleteToken((uintptr_t)lhs);
            mcc_DeleteToken((uintptr_t)rhs);
        }
        token = mcc_GetNextToken(iter);
    }
    mcc_TokenListDeleteIterator(iter);
}

mcc_TokenList_t *expandMacroFunctionTokens(preprocessor_t *preprocessor, mcc_Macro_t *macro)
{
    getToken(preprocessor);
    maybeGetWhiteSpaceToken(preprocessor);
    mcc_ExpectTokenType(preprocessor->currentToken, TOK_SYMBOL, SYM_OPEN_PAREN);
    eral_List_t *parameters = eral_ListCreate();
    getToken(preprocessor);
    maybeGetWhiteSpaceToken(preprocessor);
    bool argumentsComplete = false;
    if (preprocessor->currentToken && !(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                                        preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
    {
        mcc_TokenListIterator_t *argumentsIter = mcc_TokenListGetIterator(macro->arguments);
        while (!(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                 preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
        {
            mcc_MacroParameter_t *param = mcc_MacroParameterCreate();
            if (argumentsComplete)
                param->argument = NULL;
            else
            {
                param->argument = mcc_GetNextToken(argumentsIter);
                argumentsComplete = param->argument == NULL;
            }
            maybeGetWhiteSpaceToken(preprocessor);
            while (!(preprocessor->currentToken->tokenType == TOK_OPERATOR &&
                     preprocessor->currentToken->tokenIndex == OP_COMMA) &&
                   !(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                     preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
            {
                if (preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                    preprocessor->currentToken->tokenIndex == SYM_OPEN_PAREN)
                {
                    mcc_TokenListAppend(param->parameterTokens,
                                        mcc_CopyToken(preprocessor->currentToken));
                    getToken(preprocessor);
                    while (!(preprocessor->currentToken->tokenType == TOK_SYMBOL &&
                             preprocessor->currentToken->tokenIndex == SYM_CLOSE_PAREN))
                    {
                        mcc_TokenListAppend(param->parameterTokens,
                                            mcc_CopyToken(preprocessor->currentToken));
                        getToken(preprocessor);
                        MCC_ASSERT(preprocessor->currentToken != NULL);
                    }
                }
                mcc_TokenListAppend(param->parameterTokens,
                                    mcc_CopyToken(preprocessor->currentToken));
                getToken(preprocessor);
                MCC_ASSERT(preprocessor->currentToken != NULL);
            }
            if (preprocessor->currentToken->tokenType == TOK_OPERATOR &&
                preprocessor->currentToken->tokenIndex == OP_COMMA)
            {
                getToken(preprocessor);
            }
            eral_ListAppendData(parameters, (uintptr_t)param);
            maybeGetWhiteSpaceToken(preprocessor);
        }
        mcc_TokenListDeleteIterator(argumentsIter);
    }
    int params_len = eral_ListGetLength(parameters);
    int args_len = eral_ListGetLength(macro->arguments);
    if (macro->is_variadic)
    {
        if (params_len < args_len)
        {
            mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                            preprocessor->currentToken->lineno,
                            preprocessor->currentToken->line_index,
                            "macro function '%s' expects at least "
                            "%d argument(s), but only got "
                            "%d\n",
                            macro->text, args_len, params_len);
        }
    }
    else if (params_len != args_len)
    {
        mcc_PrettyError(mcc_ResolveFileNameFromNumber(preprocessor->currentToken->fileno),
                        preprocessor->currentToken->lineno, preprocessor->currentToken->line_index,
                        "macro function '%s' expects %d argument(s), but got %d\n", macro->text,
                        args_len, params_len);
    }
    mcc_TokenList_t *result = getReplacedMacroFunctionTokens(macro, parameters);
    eral_ListDelete(parameters, mcc_MacroParameterDelete);
    rescanMacroFunctionForActions(result);
    return result;
}