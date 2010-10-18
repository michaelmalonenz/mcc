#!/usr/bin/ruby -w

require 'fileutils'

BIN_DIR = 'bin'
TEMP_STDERR_FILE = 'stderr'

CFLAGS = "-Wall -Wextra -Werror -g -ggdb3"

CC = "/usr/bin/gcc"

def c_to_o(file)
   return file.gsub(%r{ \.c$ }x,".o")
end

def run_command(cmd, failure_message)
   result = `#{cmd}  2>#{TEMP_STDERR_FILE}`
   if $?.exitstatus != 0
      STDERR.puts failure_message
      STDERR.puts( IO.read(TEMP_STDERR_FILE))
      FileUtils.rm(TEMP_STDERR_FILE)
      exit($?.exitstatus)
   end
   FileUtils.rm(TEMP_STDERR_FILE)
end

def clean()
   FileUtils.rm_rf(BIN_DIR)
end

if $0 == __FILE__ then

   ARGV.each do |arg|
      if arg =~ %r{--?c(?:l(?:e(?:a(?:n)?)?)?)?}ix
         clean()
         exit(0)
      end
   end

   FileUtils.mkdir(BIN_DIR) unless FileTest.exist?(BIN_DIR)
   o_files = []
   Dir.glob("*.c").each do |file|
      o_file = c_to_o(file)
      o_files << o_file
      run_command("#{CC} #{CFLAGS} -c #{file} -o #{BIN_DIR}/#{o_file}", 
                  "Compilation of #{file} failed...")
   end
   Dir.chdir(BIN_DIR) do
      run_command("#{CC} #{o_files.join(' ')} -o mcc", "Linking mcc Failed...")
   end

end
