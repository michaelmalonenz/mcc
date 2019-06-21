all:
	ruby -w Make.rb

clean:
	ruby -w Make.rb --clean

tests-minimal:
	ruby -w Make.rb --no-valgrind

coverage: clean
	ruby -w Make.rb --coverage
	google-chrome bin/coverage/index.html

dogfood:
	ruby -w Make.rb --dog-food
