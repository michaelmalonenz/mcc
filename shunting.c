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
