#!/usr/bin/ruby -w

class ProcessPool

   def initilize(max_procs=10)
   end

   def run_command(cmd, args)
      return 1
   end

end


if $0 == __FILE__ then
require 'test/unit'

   class TestProcessPool < Test::Unit::TestCase

      def test_Init
         pool = ProcessPool.new
         pid = pool.run_command("ls", ['-l'])
         assert(pid != nil)
      end

   end

end
