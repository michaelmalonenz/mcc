#ifndef MCC_MACRO_H
#define MCC_MACRO_H

typedef struct macro {
	char *text;
	//char *args; ??
	int type; //float, int, unsigned long etc...
	struct macro *left;
	struct macro *right;
}mcc_Macro_t;

#define MCC_MACRO_VALUE_DEFINED "1"
#define MCC_MACRO_VALUE_UNDEFINED "0"

//hmmm need more thought putting into the type column
void mcc_DefineMacro(char *text, char *value, int type);

void mcc_UndefineMacro(char *text);

mcc_Macro_t *mcc_ResolveMacro(const char *text);

#endif /* MCC_MACRO_H */
