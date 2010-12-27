#!/usr/bin/ruby -w

require 'fileutils'
require 'set'

BIN_DIR = "#{File.expand_path(File.dirname($0))}/bin"
TEST_BIN_DIR = "#{BIN_DIR}/test"

SRC_DIR = File.expand_path(File.dirname($0))
TEST_SRC_DIR = "#{SRC_DIR}/test"

TEMP_STDERR_FILE = 'stderr'

CFLAGS = "-Wall -Wextra -Werror -g -ggdb3 -Os"

CC = "/usr/bin/gcc"

class SimpleLogger

   def initialize(filename="#{File.basename($0)}.log",
                  to_screen=true)
      @log = File.open(filename, 'w+')
      @count = 0
      @to_screen = to_screen
   end

   def note(string)
      log_string(string+"\n")
   end

   def print(string)
      log_string(string)
   end

   def error(string)
      log_string(string+"\n", STDERR)
   end

   def close
      @log.close()
   end

   private
   def log_string(string, out_stream=STDOUT)
      @log.print(string)
      out_stream.print(string) if @to_screen
      @count += 1
      if @count % 5 == 0
         @log.flush()
      end
      STDOUT.flush()
   end

end

class BuildError < Exception
end

class Linker

   FILE_SYMBOL_REGEX = %r{^(.*?):[a-f0-9]*\s+\w\s+(\w+?)$}i

   def initialize
      @symbols = {}
   end

   # For every file in the directory which ends in .o
   # find which symbols they define and add them to the class's hash
   def add_bin_dir(dir)
      files = nil
      dir = File.expand_path(dir)
      Dir.chdir(dir) do
         files = Dir.glob('*.o')
         `nm --extern-only --defined-only -A #{files.join(' ')}`.each_line do |line|
            if (line =~ FILE_SYMBOL_REGEX)
               @symbols[$2] = File.expand_path($1)
            end
         end
      end
   end

   def discover_required_files(o_file)
      o_file = File.expand_path(o_file)
      needed_symbols = find_required_symbols(o_file)
      required_files = Set.new
      unsearched_files = [o_file]
      while(unsearched_files.size > 0) do
         file = unsearched_files.shift
         next if (required_files.include? file)
         find_required_symbols(file).each do |sym|
            unsearched_files << @symbols[sym] if (@symbols.has_key?(sym))
         end
         required_files << file
      end
      return required_files.to_a
   end

   private
   def find_required_symbols(o_file)
      symbols = Set.new()
      `nm --extern-only --undefined-only -A #{o_file}`.each_line do |line|
         if (line =~ FILE_SYMBOL_REGEX)
            symbols << $2
         end
      end
      return symbols
   end

end

def c_to_o(file)
   return file.gsub(%r{ \.c$ }x,".o")
end

def run_command(cmd, failure_message)
   $log_file.note(cmd)
   result = `#{cmd}  2>#{TEMP_STDERR_FILE}`
   if $?.exitstatus != 0
      message = "Error whilst running command: '#{cmd}'
#{failure_message}
#{IO.read(TEMP_STDERR_FILE)}"
      FileUtils.rm(TEMP_STDERR_FILE)
      raise BuildError.new(message)
   end
   FileUtils.rm(TEMP_STDERR_FILE)
end

def clean()
   FileUtils.rm_rf(BIN_DIR)
end

def compile_a_directory(input_dir, out_dir)
   o_files = []
   Dir.chdir(input_dir) do
      Dir.glob("*.c").each do |file|
         o_file = c_to_o(file)
         o_files << o_file
         run_command("#{CC} #{CFLAGS} -I#{SRC_DIR} -c #{file} -o #{out_dir}/#{o_file}", 
                     "Compilation of #{input_dir}/#{file} failed...")
      end
   end
end


if $0 == __FILE__ then

   $log_file = SimpleLogger.new('mcc.log')

   ARGV.each do |arg|
      if arg =~ %r{--?c(?:l(?:e(?:a(?:n)?)?)?)?}ix
         clean()
         exit(0)
      end
   end


   $log_file.note "Initialising..."

   #These options, while better for finding specific errors, are really slow when it comes to just running the test suite.
   VALGRIND_COMMAND = "valgrind -v --leak-check=full --track-origins=yes --error-exitcode=1 --read-var-info=yes --show-reachable=yes"
#   VALGRIND_COMMAND = "valgrind -v --leak-check=full --error-exitcode=1 --show-reachable=yes"

   FileUtils.mkdir(BIN_DIR, :mode => 0775) unless FileTest.exist?(BIN_DIR)
   FileUtils.mkdir(TEST_BIN_DIR, :mode => 0775) unless FileTest.exist?(TEST_BIN_DIR)

   begin
      $log_file.note("Compiling...")
      compile_a_directory(SRC_DIR, BIN_DIR)
      compile_a_directory(TEST_SRC_DIR, TEST_BIN_DIR)

      linker = Linker.new()
      linker.add_bin_dir(BIN_DIR)

      $log_file.note("Calculating dependencies...")
      Dir.chdir(BIN_DIR) do
         dependencies = linker.discover_required_files('mcc.o')
         $log_file.note("Linking main program...")
         run_command("#{CC} #{dependencies.join(' ')} -o mcc", "Linking mcc Failed...")
      end

      Dir.chdir(TEST_BIN_DIR) do
         $log_file.note("Linking Tests...")
         Dir.glob("test_*.o").each do |test_exe_o|
            test_exe = test_exe_o.gsub(/\.o$/, '')
            dependencies = linker.discover_required_files(test_exe_o)
            run_command("#{CC} #{dependencies.join(' ')} -o #{test_exe}", 
                        "Linking #{test_exe} Failed...")
         end

         $log_file.note("Running tests...")
         Dir.new(TEST_BIN_DIR).each do |file|
            if !FileTest.directory?(file) && FileTest.executable?(file)
               run_command("#{VALGRIND_COMMAND} ./#{file}", "#{File.basename(file)} failed to run correctly!")
            end
         end
      end
   rescue Exception => error
      $log_file.error(error.to_s)         
      $log_file.close()
      exit(1)
   end

   $log_file.note("All build actions completed successfully!")
   $log_file.close
end
