
require 'pp'

Oper = Struct.new(:pri,:sym,:type)

class TreeOutput
  def initialize
    @vstack = []
  end

  def oper( op)
    rightv = @vstack.pop
    raise "Missing value in expression" if !rightv
    if (op.sym == :comma || op.sym == :call) && rightv.is_a?(Array) && rightv[0] == :comma
      # This is a way to flatten the tree by removing all the :comma operators
      @vstack << [op.sym,@vstack.pop] + rightv[1..-1]
    else
      if op.type == :infix
        leftv = @vstack.pop
        raise "Missing value in expression" if !leftv
        @vstack << [op.sym, leftv, rightv]
      else
        @vstack <<  [op.sym,rightv]
      end
    end
  end

  def value v; @vstack << v; end

  def result
    raise "Incomplete expression - #{@vstack.inspect}" if @vstack.length != 1
    return @vstack[0]
  end
end

class RPNOutput
  def initialize; @rpn = []; end
  def oper o;     @rpn << o.sym; end
  def value v;    @rpn << v; end
  def result;     @rpn; end
end

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

def shunt a
  opers = {
    "+" => Oper.new(10, :plus,  :infix),
    "++" => {:infix_or_postfix => Oper.new(30, :postincr,  :postfix), 
             :prefix => Oper.new(30,:preincr, :prefix)},
    "-" => Oper.new(10, :minus, :infix),
    "*" => Oper.new(20, :mul,   :infix),
    "/" => Oper.new(20, :div,   :infix),
    "!" => Oper.new(30, :not,   :prefix),
    "," => Oper.new(2,  :comma,   :infix),
    "(" => Oper.new(99, nil,   :lp),
    ")" => Oper.new(99, nil,   :rp)
  }

  ShuntingYard.new(opers,TreeOutput.new).shunt(SimpleLexer.new(a)).result
end

class SimpleLexer
  def initialize s; @s = s; end

  def each
    @s.scan(/[ \r\n]*([0-9]+|[A-Za-z]+|\+\+|[\(\)+\-*\/\!,])[ \r\n]*/).each do |token|
      token = token[0]
      yield((?0 .. ?9).member?(token[0]) ? token.to_i : token)
    end
  end
end

PP.pp shunt("1 + !5")
PP.pp shunt("1 + 5")
PP.pp shunt("1 + 2 * 3")
PP.pp shunt("1 * 2 + 3 / 5")
PP.pp shunt("(1 + 2) * 3")
PP.pp shunt("3 * (1 + 2)")
PP.pp shunt("3 * (1 + (2 * 4))")
PP.pp shunt("f(1)")
PP.pp shunt("f(1,2)")
PP.pp shunt("f(1,2,3)")
PP.pp shunt("1 * f ++ + 5")
PP.pp shunt("++f")
PP.pp shunt("1 + ++f")
PP.pp shunt("1 + f ++ - f")
PP.pp shunt("1 + ! 5 * 2")
begin
  PP.pp shunt("f + + 5") # Makes no sense. Should give an error.
rescue
  puts "Failed, as it should"
end

