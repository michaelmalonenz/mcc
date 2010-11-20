#ifndef MCC_MACRO_H
#define MCC_MACRO_H

typedef struct macro {
	char *text;
	char *value;
	//char *args; ??
	struct macro *left;
	struct macro *right;
}mcc_Macro_t;

void mcc_DefineMacro(char *text, char *value);

void mcc_UndefineMacro(char *text);

mcc_Macro_t *mcc_ResolveMacro(const char *text);

char *mcc_DoMacroReplacement(char *text);

#endif /* MCC_MACRO_H */
