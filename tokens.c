/**
 * This is a C File to hold the variables which contain the strings of keywords, operators and symbols
 * This is to make linkage easier than if they were in a header file (which needs to be #include'd
 * in several places)
 */
#include "tokens.h"

/* The following char *[]'s need to be kept in sync with the corresponding enums in tokens.h */

const char *keywords[NUM_KEYWORDS] = { "auto", "break", "case", "char", "const", "continue", "default", "do",
									   "double", "else", "enum", "extern", "float", "for", "goto", "if",
									   "int", "long", "register", "return", "short", "signed", "sizeof", "static",
									   "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
#if MCC_C99_COMPATIBLE
									   ,"_Bool", "_Complex", "_Imaginary", "inline", "restrict"
#endif
};

char *operators[NUM_OPERATORS] = { "+", "-", "/", "*", "%", "--", "++",
								   "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=",
								   "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!",
								   "&", "|", "^", "<<", ">>", "~",
								   "sizeof", "&", "*", "?", ":", "," };

char *symbols[NUM_SYMBOLS] = { "{", "}", "[", "]", ";", "(", ")", "\"", "'", "\\" };
