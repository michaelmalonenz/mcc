#!/usr/bin/ruby -w

#    mcc a lightweight compiler for developers, not machines
#    Copyright (C) 2011 Michael Malone
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

require 'fileutils'
require 'set'

BIN_DIR = "#{File.expand_path(File.dirname($0))}/bin"
TEST_BIN_DIR = "#{BIN_DIR}/test"

SRC_DIR = File.expand_path(File.dirname($0))
TEST_SRC_DIR = "#{SRC_DIR}/test"

TEMP_STDERR_FILE = 'stderr'

CFLAGS = "-Wall -Wextra -Werror -g -ggdb3 -O0"
LINKER_FLAGS = "-lm -W -Wall"

$cc = "/usr/bin/gcc"

MAIN_OBJECT_FILE = "mcc.o"
MAIN_EXE_NAME = "mcc"

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

class CompileJob

   @@known_files = []

   def initialize(command, failure_message=nil, cleanup_file=nil)
      @command = command
      @failure_message = failure_message
   end

   def CompileJob.clean_unused_o_files(out_dir)
      Dir.chdir(out_dir) do
         o_files_to_delete = Dir.glob('*.o') - @@known_files
         FileUtils.rm(o_files_to_delete)
      end
   end

   def compile!
      run_command(@command, @failure_message)
   end

   def CompileJob::known_files
      @@known_files
   end

end

def c_to_o(file)
   return file.gsub(%r{ \.c$ }x,".o")
end

def run_command(cmd, failure_message)
   $log_file.note(cmd)
   result = `#{cmd}  2>#{TEMP_STDERR_FILE}`
   if $?.exitstatus != 0
      puts result
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
   jobs = []
   Dir.chdir(input_dir) do
      Dir.glob("*.c").each do |file|
         o_file = c_to_o(file)
         out_file = "#{out_dir}/#{o_file}"
         CompileJob::known_files << o_file
         if !FileTest.exist?(out_file) || File.mtime(file) > File.mtime(out_file)
            jobs << CompileJob.new( "#{$cc} #{CFLAGS} -I#{SRC_DIR} -c #{file} \
-o #{out_dir}/#{o_file}",
                                    "Compilation of #{input_dir}/#{file} failed...",
                                    o_file)
         end
      end
      jobs.each do |job|
         job.compile!
      end
   end
   CompileJob.clean_unused_o_files(out_dir)
end


if $0 == __FILE__ then
   #These options, while better for finding specific errors, are really slow when it comes to just running the test suite.
#   VALGRIND_COMMAND = "valgrind -v --leak-check=full --track-origins=yes --error-exitcode=1 --read-var-info=yes --show-reachable=yes"
   VALGRIND_COMMAND = "valgrind --leak-check=full --error-exitcode=1 --show-reachable=yes"


   tests = []
   $log_file = SimpleLogger.new("#{MAIN_EXE_NAME}.log")
   dog_food = false
   coverage = false
   valgrind_cmd = VALGRIND_COMMAND

   ARGV.each do |arg|
      if arg =~ %r{--?cl(?:e(?:a(?:n)?)?)?}ix
         clean()
         exit(0)
      elsif arg =~ %r{--?t(?:e(?:s(?:t(?:[-_](?:o(?:n(?:ly)?)?)?)?)?)?)?=(.+)}ix
         tests << $1
      elsif arg =~ %r{--?d(?:o(?:g(?:f(?:o(?:od)?)?)?)?)?}ix
         dog_food = true
      elsif arg =~ %r{--?co(?:v(?:e(?:r(?:a(?:ge)?)?)?)?)?}ix
         coverage = true
         CFLAGS << " --coverage -fprofile-arcs -ftest-coverage"
         LINKER_FLAGS << " --coverage"
      elsif arg =~ %r{--?n(?:o-?(?:v(?:a(?:l(?:g(?:r(?:i(?:n(?:d)?)?)?)?)?)?)?)?)?}ix
         valgrind_cmd = ''         
      end
   end


   $log_file.note "Initialising..."

   FileUtils.mkdir(BIN_DIR, :mode => 0775) unless FileTest.exist?(BIN_DIR)
   FileUtils.mkdir(TEST_BIN_DIR, :mode => 0775) unless FileTest.exist?(TEST_BIN_DIR)

   begin
      $log_file.note("Compiling...")
      compile_a_directory(SRC_DIR, BIN_DIR)
      compile_a_directory(TEST_SRC_DIR, TEST_BIN_DIR)

      $log_file.note("Finished Compilation")
      linker = Linker.new()
      linker.add_bin_dir(BIN_DIR)

      $log_file.note("Calculating dependencies...")
      Dir.chdir(BIN_DIR) do
         dependencies = linker.discover_required_files(MAIN_OBJECT_FILE)
         $log_file.note("Linking main program...")
         run_command("#{$cc} #{dependencies.join(' ')} #{LINKER_FLAGS} -o #{MAIN_EXE_NAME}", "Linking #{MAIN_EXE_NAME} Failed...")
      end

      if coverage
         $log_file.note("Cleaning out old coverage results")
         Dir.chdir(BIN_DIR) do
            Dir.new(BIN_DIR).each do |file|
               if file =~ %r{\.gcda$}i
                  FileUtils.rm(file)
               end
            end
            run_command("lcov -c -i -b #{SRC_DIR} -d #{SRC_DIR} -o mcc_baseline.info --ignore-errors source",
                        "Couldn't create coverage baseline")
         end
      end


      Dir.chdir(TEST_BIN_DIR) do
         linker.add_bin_dir(TEST_BIN_DIR)
         $log_file.note("Linking Tests...")
         Dir.glob("test_*.o").each do |test_exe_o|
            test_exe = test_exe_o.gsub(/\.o$/, '')
            dependencies = linker.discover_required_files(test_exe_o)
            run_command("#{$cc} #{dependencies.join(' ')} #{LINKER_FLAGS} -o #{test_exe}",
                        "Linking #{test_exe} Failed...")
         end

         $log_file.note("Running tests...")
         if tests.empty?
            Dir.new(TEST_BIN_DIR).each do |file|
               if !FileTest.directory?(file) && FileTest.executable?(file)
                  run_command("#{valgrind_cmd} ./#{file}", "#{File.basename(file)} failed to run correctly!")
               end
            end
         else
            tests.each do |test|
               if !FileTest.directory?(test) && FileTest.executable?(test)
                  run_command("#{valgrind_cmd} ./#{test}", "#{File.basename(test)} failed to run correctly!")
               end
            end
         end
      end

      if coverage
         Dir.chdir(BIN_DIR) do
            run_command("lcov -d #{SRC_DIR} -b #{SRC_DIR} -c -o mcc_test_results.info --ignore-errors source",
                        "Failed to measure test coverage")
            run_command("lcov -r mcc_test_results.info test_*.c -o mcc_test_results.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -r mcc_test_results.info TestUtils.c -o mcc_test_results.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -r mcc_test_results.info mcc.c -o mcc_test_results.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -r mcc_baseline.info test_*.c -o mcc_baseline.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -r mcc_baseline.info TestUtils.c -o mcc_baseline.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -r mcc_baseline.info mcc.c -o mcc_baseline.info",
                        "Failed to remove test results from coverage results")
            run_command("lcov -a mcc_baseline.info -a mcc_test_results.info -o total.info",
                        "Failed to generate coverage html")
            run_command("genhtml total.info -o coverage",
                        "Failed to generate coverage html")
         end
      end

      if dog_food
         $log_file.note("Eating My Own Dog Food...")
         # use vlagrind...
         $cc = "#{BIN_DIR}/#{MAIN_EXE_NAME}"
         compile_a_directory(SRC_DIR, BIN_DIR)
         compile_a_directory(TEST_SRC_DIR, TEST_BIN_DIR)
      end

   rescue Exception => error
      $log_file.error(error.to_s)
      $log_file.close()
      exit(1)
   end

   $log_file.note("All build actions completed successfully!")
   $log_file.close
end
