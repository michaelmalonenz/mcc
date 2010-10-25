#include <stdlib.h>
#include "token.h"

static mcc_Token_t *firstToken = NULL;

//I'm not convinced currentToken is the best name for this
static mcc_Token_t *currentToken = NULL;

static mcc_Token_t *nextToken = NULL;

//Does this imply that I should really have an opaque type?
mcc_Token_t *mcc_CreateToken()
{
	mcc_Token_t *result = (mcc_Token_t *) malloc(sizeof(mcc_Token_t));
	return result;
}

void mcc_DeleteToken(mcc_Token_t *token)
{
	//This isn't complete, need to worry about the name as well
	free(token);
}

void mcc_AddToken(mcc_Token_t *token)
{
	if (firstToken == NULL)
	{
		firstToken = token;
	}
	token->next = currentToken;
	currentToken = token;
}

void mcc_FreeTokens()
{
	mcc_Token_t *temp = NULL;
	if(currentToken == NULL)
		return;
	temp = currentToken->next;
	while(currentToken != NULL)
	{
		mcc_DeleteToken(currentToken);
		currentToken = temp;
	}
}

mcc_Token_t *mcc_GetNextToken()
{
	mcc_Token_t *result;
	if (nextToken == NULL)
	{
		nextToken = firstToken;
	}
	if (nextToken == NULL)
	{
		result = NULL;
	}
	else
	{
		result = nextToken;
		nextToken = nextToken->next;
	}
	return result;
}


