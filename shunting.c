/*
class ShuntingYard
  def initialize opers,output
    @ostack,@opers,@out = [],opers,output
  end

  def reduce op = nil
    pri = op ? op.pri : 0
    # We check for :postfix to handle cases where a postfix operator has been given a lower precedence than an
    # infix operator, yet it needs to bind tighter to tokens preceeding it than a following infix operator regardless,
    # because the alternative gives a malfored expression.
    while  !@ostack.empty? && (@ostack[-1].pri > pri || @ostack[-1].type == :postfix)
      o = @ostack.pop
      return if o.type == :lp
      @out.oper(o)
    end
  end

  def shunt src
    possible_func = false     # was the last token a possible function name?
    opstate = :prefix         # IF we get a single arity operator right now, it is a prefix operator
                              # "opstate" is used to handle things like pre-increment and post-increment that
                              # share the same token.
    src.each do |token|
      if op = @opers[token]
        op = op[opstate] if op.is_a?(Hash)
        if op.type == :rp then reduce
        else
          opstate = :prefix
          reduce op # For handling the postfix operators
          @ostack << (op.type == :lp && possible_func ? Oper.new(1, :call, :infix) : op)
          o = @ostack[-1]
        end
      else 
        @out.value(token)
        opstate = :infix_or_postfix # After a non-operator value, any single arity operator would be either postfix,
                                    # so when seeing the next operator we will assume it is either infix or postfix.
      end
      possible_func = !op && !token.is_a?(Numeric)
    end
    reduce
    
    return @out if  @ostack.empty?
    raise "Syntax error. #{@ostack.inspect}"
  end
end
*/
#include "mcc.h"
#include "tokenList.h"
#include "stack.h"

typedef enum op_state { PREFIX, INFIX, POSTFIX, INFIX_OR_POSTFIX } op_state_t;

static void reduce(mcc_TokenListIterator_t UNUSED(*iter), mcc_Stack_t UNUSED(*output),
                   mcc_Stack_t UNUSED(*operator_stack), const mcc_Token_t UNUSED(*currentTok))
{
}

static void mcc_ResolveOperator(mcc_Token_t UNUSED(*token), op_state_t UNUSED(opstate))
{
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
         reduce(iter, output, operator_stack, NULL);
      }
      else if (token->tokenType == TOK_OPERATOR)
      {
         //Yes, it is naughty to cast away the const, but the Lexer alone can't tell whether
         //any given operator is prefix, infix or postfix
         mcc_ResolveOperator((mcc_Token_t *) token, opstate);
         opstate = PREFIX;
         reduce(iter, output, operator_stack, token);
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
         //possibleFunction = mcc_IsDefinedFunction(token);
         opstate = INFIX_OR_POSTFIX;
         //resolve the id then push onto the stack
         mcc_StackPush(output, (uintptr_t) token);
      }
      else
      {
#if MCC_DEBUG
         if (token->tokenType != TOK_WHITESPACE)
         {
            MCC_ASSERT(FALSE);
            mcc_DebugPrintToken(token);
         }
#endif
      }
      reduce(iter, output, operator_stack, NULL);
      
      token = mcc_GetNextToken(iter);
   }
}
