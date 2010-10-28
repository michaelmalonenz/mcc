#include <string.h>
#include <stdlib.h>

#include "macro.h"
#include "mcc.h"

static mcc_Macro_t *root = NULL;

static mcc_Macro_t *create_macro(char *text, char UNUSED(*value), int UNUSED(type))
{
	mcc_Macro_t *result = (mcc_Macro_t *) malloc(sizeof(mcc_Macro_t));
	result->text = (char *) malloc(sizeof(char) * strlen(text));
	MCC_ASSERT(result != NULL);
	MCC_ASSERT(result->text != NULL);
	strncpy(result->text, text, strlen(text));
	//store it as the type?
	return result;
}

static void delete_macro(mcc_Macro_t *macro)
{
	MCC_ASSERT(macro != NULL);
	MCC_ASSERT(macro->text != NULL);
	free(macro->text);
	free(macro);
}

void mcc_DefineMacro(char *text, char *value, int type)
{
	mcc_Macro_t *current = root;
	while (current != NULL)
	{
		switch(strncmp(text, current->text, 
					   max(strlen(current->text), strlen(text))))
		{
		case 1:
			if (current->right == NULL)
				current->right = create_macro(text, value, type);
			else
				current = current->right;
			break;
		case 0:
			mcc_Error("Macro '%s' is already defined\n", text);
			break;
		case -1:
			if (current->left == NULL)
				current->left = create_macro(text, value, type);
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
			break;
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
