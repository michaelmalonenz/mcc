#include "mcc.h"
#include "tokenList.h"
#include "stack.h"
#include "symbol_table.h"

typedef enum op_state { PREFIX, INFIX, POSTFIX, INFIX_OR_POSTFIX } op_state_t;

static void mcc_ResolveOperator(mcc_Token_t *op, op_state_t opstate)
{
   if (opstate == PREFIX)
   {
      switch(op->tokenIndex)
      {
         case OP_DECREMENT_POST:
         {
            op->tokenIndex = OP_DECREMENT_PRE;
         }
         case OP_INCREMENT_POST:
         {
            op->tokenIndex = OP_INCREMENT_PRE;
         }
         case OP_BITWISE_AND:
         {
            op->tokenIndex = OP_ADDRESS_OF;
         }
         case OP_MULTIPLY:
         {
            op->tokenIndex = OP_DEREFERENCE;
         }
      }
   }
   else
   {
      switch(op->tokenIndex)
      {
         case OP_DECREMENT_PRE:
         {
            op->tokenIndex = OP_DECREMENT_POST;
         }
         case OP_INCREMENT_PRE:
         {
            op->tokenIndex = OP_INCREMENT_POST;
         }
         case OP_ADDRESS_OF:
         {
            op->tokenIndex = OP_BITWISE_AND;
         }
         case OP_DEREFERENCE:
         {
            op->tokenIndex = OP_MULTIPLY;
         }
      }
   }
}

static int getRelativeOperatorPrecedence(MCC_OPERATOR op)
{
   static const int precedents[NUM_OPERATORS] = {
      [OP_MEMBER_OF] = 1, [OP_DEREF_MEMBER_OF] = 2,
      [OP_DECREMENT_POST] = 3, [OP_INCREMENT_POST] = 3, [OP_DEREFERENCE] = 4,
      [OP_ADDRESS_OF] = 4, [OP_NOT] = 5, [OP_NEGATE] = 6, 
      [OP_SIZEOF] = 7, [OP_MULTIPLY] = 8, [OP_DIVIDE] = 8, [OP_MODULO] = 9,
      [OP_ADD] = 10, [OP_MINUS] = 10, [OP_R_SHIFT] = 11, [OP_L_SHIFT] = 11,
      [OP_LESS_THAN] = 12, [OP_GREATER_THAN] = 12, [OP_LESS_EQUAL] = 13, 
      [OP_GREATER_EQUAL] = 13, [OP_COMPARE_TO] = 13, [OP_NOT_EQUAL] = 14,
      [OP_BITWISE_AND] = 15, [OP_BITWISE_EXCL_OR] = 16, 
      [OP_BITWISE_INCL_OR] = 17, [OP_LOGICAL_AND] = 18, 
      [OP_LOGICAL_INCL_OR] = 19, [OP_TERNARY_IF] = 20, [OP_TERNARY_ELSE] = 20,
      [OP_EQUALS_ASSIGN] = 21, [OP_PLUS_EQUALS] = 21, [OP_MINUS_EQUALS] = 21,
      [OP_TIMES_EQUALS] = 21, [OP_DIVIDE_EQUALS] = 21, [OP_MOD_EQUALS] = 21,
      [OP_R_SHIFT_EQUALS] = 21, [OP_L_SHIFT_EQUALS] = 21,
      [OP_BITWISE_AND_EQUALS] = 21, [OP_BITWISE_EXCL_OR_EQUALS] = 21,
      [OP_BITWISE_INCL_OR_EQUALS] = 21, [OP_COMMA] = 22,
      [OP_DECREMENT_PRE] = 23, [OP_INCREMENT_PRE] = 23,
   };
   
   return precedents[op];
}

static op_state_t getOperatorFixness(const mcc_Token_t UNUSED(*token))
{
   return INFIX;
}

static void reduce(mcc_Stack_t UNUSED(*output), mcc_Stack_t UNUSED(*operator_stack),
                   const mcc_Token_t *token)
{
   int precedence;
   const mcc_Token_t *temp;
   if (token == NULL)
      precedence = INT_MAX;
   else
      precedence = getRelativeOperatorPrecedence((MCC_OPERATOR) token->tokenIndex);

   /**
    * We check for :postfix to handle cases where a postfix operator has been given a lower precedence than an
    * infix operator, yet it needs to bind tighter to tokens preceeding it than a following infix operator regardless,
    * because the alternative gives a malfored expression.
    */
   temp = (mcc_Token_t *) mcc_StackPeek(operator_stack);
   while (!mcc_StackEmpty(operator_stack) && 
          (getRelativeOperatorPrecedence(temp->tokenIndex) < precedence ||
           getOperatorFixness(temp) == POSTFIX))
   {
      mcc_Token_t *op = (mcc_Token_t *) mcc_StackPop(operator_stack);
      if (op->tokenType == TOK_SYMBOL && op->tokenIndex == SYM_OPEN_PAREN)
      {
         return;
      }
      mcc_StackPush(output, (uintptr_t) op);
      temp = (mcc_Token_t *) mcc_StackPeek(operator_stack);
   }
}

void shunt(mcc_TokenListIterator_t *iter)
{
   op_state_t opstate = PREFIX;
   bool_t possibleFunction = FALSE;
   mcc_Stack_t *operator_stack = mcc_StackCreate();
   mcc_Stack_t *output = mcc_StackCreate();
   const mcc_Token_t *token = mcc_GetNextToken(iter);

   while (token->tokenType != TOK_EOL) //&& (token->tokenType != TOK_SYMBOl && token->tokenIndex == SYM_SEMI_COLON)
   {
      if (token->tokenType == TOK_SYMBOL && token->tokenIndex == SYM_CLOSE_PAREN)
      {
         reduce(output, operator_stack, NULL);
      }
      else if (token->tokenType == TOK_OPERATOR || token->tokenType == TOK_SYMBOL)
      {
         //Yes, it is naughty to cast away the const, but the Lexer alone can't tell whether
         //any given operator is prefix, infix or postfix
         mcc_ResolveOperator((mcc_Token_t *) token, opstate);
         opstate = PREFIX;
         reduce(output, operator_stack, token);
         if (possibleFunction && token->tokenType == TOK_SYMBOL &&
             token->tokenIndex == SYM_OPEN_PAREN)
         {
            //add a "call function" op to the operator_stack
         }
         else
         {
            mcc_StackPush(operator_stack, (uintptr_t) token);
         }
      }
      else if (token->tokenType == TOK_NUMBER)
      {
         opstate = INFIX_OR_POSTFIX;
         mcc_StackPush(output, (uintptr_t) token);
      }
      else if (token->tokenType == TOK_IDENTIFIER)
      {
         possibleFunction = TRUE;
         possibleFunction = mcc_IsFunction(token);
         opstate = INFIX_OR_POSTFIX;
         //resolve the id then push onto the stack
         mcc_StackPush(output, (uintptr_t) token);
      }
      else
      {
#if MCC_DEBUG
         if (token->tokenType != TOK_WHITESPACE)
         {
            mcc_DebugPrintToken(token);
            MCC_ASSERT(FALSE);
         }
#endif
      }
      reduce(output, operator_stack, NULL);
      
      token = mcc_GetNextToken(iter);
   }

   mcc_StackDelete(operator_stack, NULL);

   mcc_DebugPrintStack(output, mcc_DebugPrintToken_Fn);

   mcc_StackDelete(output, NULL);
}
