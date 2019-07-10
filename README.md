# Mike's C compiler

This is a toy compiler aimed to be a learning tool for me.  It's about figuring out how to parse
a language efficiently and correctly.  Compilers are hard to write - they involve a surprisingly
large amount of computer science theory and doing it in C is another level of challenge, as I also
have to implement some fundamental datastructures, such as hash tables, lists, stacks... etc.

I aim to eventually get as far as outputting LLVM bitcode and run the program on the LLVM interpreter.

## Aims
- Self-compile my compiler
- Be as gcc / clang compatible as possible
- Stretch Goal: Compile the linux kernel
