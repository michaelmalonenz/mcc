# Mike's C compiler

This is a toy compiler aimed to be a learning tool for me.  It's about figuring out how to parse
a language efficiently and correctly.  Compilers are hard to write - they involve a surprisingly
large amount of computer science theory and doing it in C is another level of challenge, as I also
have to implement some fundamental datastructures, such as hash tables, lists, stacks... etc.

I know that I'm doing things a bit more manually than I have to (e.g. hand-rolling a recursive descent
parser) but that's how I want to do things - the aim is not to make the worlds most commercially successful
compiler, but to make one that helps me to understand what goes on in parsing.

I aim to eventually get as far as outputting LLVM bitcode and run the program on the LLVM interpreter.

## Aims
- Self-compile my compiler
- Be as gcc / clang compatible as possible (cli switch-wise)
- Learn stuff
