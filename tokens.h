#ifndef MCC_TOKENS_H_
#define MCC_TOKENS_H_

/* here are some extra keywords I should probably also support:
     DEF(TOK_CONST2, "__const")
     DEF(TOK_CONST3, "__const__")
     DEF(TOK_VOLATILE2, "__volatile")
     DEF(TOK_VOLATILE3, "__volatile__")
     DEF(TOK_SIGNED2, "__signed")
     DEF(TOK_SIGNED3, "__signed__")
     DEF(TOK_INLINE2, "__inline")
     DEF(TOK_INLINE3, "__inline__")
     DEF(TOK_RESTRICT2, "__restrict")
     DEF(TOK_RESTRICT3, "__restrict__")
     DEF(TOK_EXTENSION, "__extension__")
     DEF(TOK_ATTRIBUTE1, "__attribute")
     DEF(TOK_ATTRIBUTE2, "__attribute__")
     DEF(TOK_ALIGNOF1, "__alignof")
     DEF(TOK_ALIGNOF2, "__alignof__")
     DEF(TOK_TYPEOF1, "typeof")
     DEF(TOK_TYPEOF2, "__typeof")
     DEF(TOK_TYPEOF3, "__typeof__")
     DEF(TOK_LABEL, "__label__")
     DEF(TOK_ASM1, "asm")
     DEF(TOK_ASM2, "__asm")
     DEF(TOK_ASM3, "__asm__")
*/
#include "mcc.h"

typedef struct token {
	char *name;
	int type;
	bool_t isKeyword;
	bool_t isSymbol;
	bool_t isOperator;
	unsigned short fileno;
	int lineno;
	struct token *next;
} mcc_Token_t;


const char *keywords[] = { "auto", "break", "case", "char", "const", "continue", "default", "do",
							 "double", "else", "enum", "extern", "float", "for", "goto", "if",
							 "int", "long", "register", "return", "short", "signed", "sizeof", "static",
							 "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

enum key_index { KEY_AUTO, KEY_BREAK, KEY_CASE, KEY_CHAR, KEY_CONST, KEY_CONTINUE, KEY_DEFAULT, KEY_DO, KEY_DOUBLE,
				 KEY_ELSE, KEY_ENUM, KEY_EXTERN, KEY_FLOAT, KEY_FOR, KEY_GOTO, KEY_IF, KEY_INT, KEY_LONG,
				 KEY_REGISTER, KEY_RETURN, KEY_SHORT, KEY_SIGNED, KEY_SIZEOF, KEY_STATIC, KEY_STRUCT,
				 KEY_SWITCH, KEY_TYPEDEF, KEY_UNION, KEY_UNSIGNED, KEY_VOID, KEYVOLATILE, KEY_WHILE, NUM_KEYWORDS};

const char *c99_keywords[] = { "_Bool", "_Complex", "_Imaginary", "inline", "restrict" };
enum c99_key_index { C99_KEY_BOOL, C99_KEY_COMPLEX, C99_KEY_IMAGINARY, C99_KEY_INLINE, C99_KEY_RESTRICT, NUM_C99_KEYWORDS};

char *operators[] = { "+", "-", "/", "*", "%", "--", "++",
					  "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=",
					  "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!",
					  "&", "|", "^", "<<", ">>", "~",
					  "sizeof", "&", "*", "?", ":", "," };

enum operator_index {OP_ADD, OP_MINUS, OP_DIVIDE, OP_MULTIPLY, OP_MODULO, OP_DECREMENT, OP_INCREMENT,
					 OP_EQUALS_ASSIGN, OP_TIMES_EQUALS, OP_DIVIDE_EQUALS, OP_MOD_EQUALS, OP_PLUS_EQUALS,
					 OP_MINUS_EQUALS, OP_L_SHIFT_EQUALS, OP_R_SHIFT_EQUALS, OP_BITWISE_AND_EQUALS,
					 OP_BITWISE_EXCL_OR_EQUALS, OP_BITWISE_INCL_OR_EQUALS, OP_COMPARE_TO, OP_NOT_EQUAL,
					 OP_GREATER_THAN, OP_LESS_THAN, OP_GREATER_EQUAL, OP_LESS_EQUAL, OP_LOGICAL_AND,
					 OP_LOGICAL_EXCL_OR, OP_LOGICAL_INCL_OR, OP_NOT, OP_BITWISE_AND, OP_BITWISE_INCL_OR,
					 OP_BITWISE_EXCL_OR, OP_L_SHIFT, OP_R_SHIFT, OP_NEGATE, OP_SIZEOF, OP_ADDRESS_OF, OP_TERNARY_IF,
					 OP_TERNARY_ELSE, OP_COMMA, NUM_OPERATORS};

char *symbols[] = { "{", "}", "[", "]", ";", "(", ")", "\"", "'", "\\" };
enum symbol_index {SYM_OPEN_BRACE, SYM_CLOSE_BRACE, SYM_OPEN_BRACKET, SYM_CLOSE_BRACKET, SYM_SEMI_COLON, 
				   SYM_OPEN_PAREN, SYM_CLOSE_PAREN, SYM_DOUBLE_QUOTE, SYM_SINGLE_QUOTE, SYM_ESCAPE, NUM_SYMBOLS};

#endif /* MCC_TOKENS_H_ */
