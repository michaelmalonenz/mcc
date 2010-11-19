#include <string.h>
#include <stdlib.h>

#include "macro.h"
#include "mcc.h"

static mcc_Macro_t *root = NULL;

// This step should involve macro replacement, so we only have to
// process each macro once - as a side-effect it will correct a bug
// I would have introduced which would define the macro as the latest
// version of it, which is not necessarily the intended one.
// #define SOME_MACRO (3)
// #define SOME_OTHER_MACRO SOME_MACRO (SOME_MACRO + 4)
// #undef SOME_MACRO
// #define SOME_MACRO (6)
// #define YET_ANOTHER_MACRO (SOME_MACRO + 4)
// If we delayed parsing SOME_OTHER_MACRO until we found each occurence, it 
// would yield the same result as YET_ANOTHER_MACRO, which is bad!
//
// To make life better, I also need to evaluate constant expressions here
static mcc_Macro_t *create_macro(char *text, char UNUSED(*value))
{
	mcc_Macro_t *result = (mcc_Macro_t *) malloc(sizeof(mcc_Macro_t));
	result->text = (char *) malloc(sizeof(char) * strlen(text));
	MCC_ASSERT(result != NULL);
	MCC_ASSERT(result->text != NULL);
	strncpy(result->text, text, strlen(text));
	result->left = NULL;
	result->right = NULL;
	return result;
}

static void delete_macro(mcc_Macro_t *macro)
{
	MCC_ASSERT(macro != NULL);
	MCC_ASSERT(macro->text != NULL);
	free(macro->text);
	free(macro);
}

void mcc_DefineMacro(char *text, char *value)
{
	mcc_Macro_t *current = root;
	if (root == NULL)
	{
		root = create_macro(text, value);
		return;
	}
		
	while (current != NULL)
	{
		switch(strncmp(text, current->text, 
					   max(strlen(current->text), strlen(text))))
		{
		case 1:
			if (current->right == NULL)
				current->right = create_macro(text, value);
			else
				current = current->right;
			break;
		case 0:
			mcc_Error("Macro '%s' is already defined\n", text);
			break;
		case -1:
			if (current->left == NULL)
				current->left = create_macro(text, value);
			else
				current = current->left;
			break;
		}
	}
}

void mcc_UndefineMacro(char *text)
{
	mcc_Macro_t *current = root;
	while (current != NULL)
	{
		switch(strncmp(text, current->text, 
					   max(strlen(current->text), strlen(text))))
		{
		case 1:
			current = current->right;
			break;
		case 0:
			//repair the b-tree
			delete_macro(current);
			return;
		case -1:
			current = current->left;
			break;
		}
	}
}

mcc_Macro_t *mcc_ResolveMacro(const char *text)
{
	mcc_Macro_t *current = root;
	while (current != NULL)
	{
		switch(strncmp(text, current->text, 
					   max(strlen(current->text), strlen(text))))
		{
		case 1:
			current = current->right;
			break;
		case 0:
			return current;
			break;
		case -1:
			current = current->left;
			break;
		}
	}
	return NULL;
}
