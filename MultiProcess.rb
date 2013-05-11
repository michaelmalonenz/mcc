#!/usr/bin/ruby -w

class ProcessPool

   @@num_cores = `cat /proc/cpuinfo | grep processor | wc -l`.to_i

   def initilize(max_procs=@@num_cores+1)
   end

   def run_command(cmd, args)
      return spawn(cmd + " " + args.join(" "))
   end

end


if $0 == __FILE__ then
require 'test/unit'

   class TestProcessPool < Test::Unit::TestCase

      def test_Init
         pool = ProcessPool.new
         pid = pool.run_command("ls", ['-l'])
         assert(pid != nil)
         return Process.waitpid(pid)
      end

   end

end
