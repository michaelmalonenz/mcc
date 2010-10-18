#!/usr/bin/ruby -w

require 'fileutils'

BIN_DIR = 'bin'
SRC_DIR = Dir.pwd

C_FILES = %w{ mcc.c tokeniser.c }
H_FILES = %w{ tokens.h mcc.h }

CFLAGS = "-Wall -Wextra -Werror -g -ggdb3"

CC = "/usr/bin/gcc"

def c_to_o(file)
   return file.gsub(%r{ \.c$ }x,".o")
end

def run_command(cmd, failure_message)
   result = `#{cmd}`
   if $?.exitstatus != 0
      STDERR.puts failure_message
      STDERR.puts( IO.read('stderr'))
      exit($?.exitstatus)
   end
end

FileUtils.mkdir(BIN_DIR) unless FileTest.exist?(BIN_DIR)
o_files = []
C_FILES.each do |file|
   o_file = c_to_o(file)
   o_files << o_file
   run_command("#{CC} #{CFLAGS} -c #{file} -o #{BIN_DIR}/#{o_file} 2>stderr", 
               "Compilation of #{file} failed...")
end
Dir.chdir(BIN_DIR) do
   run_command("#{CC} #{o_files.join(' ')} -o mcc", "Linking mcc Failed...")
end
