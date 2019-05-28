all:
	ruby -w Make.rb

clean:
	ruby -w Make.rb --clean

tests-minimal:
	ruby -w Make.rb --no-valgrind
