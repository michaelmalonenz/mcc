/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include <stdlib.h>

#include "mcc.h"
#include "tokens.h"

int main(int argc, char **argv)
{
   int i;
   
   if (argc <= 1)
   {
      mcc_Error("No input files\n");
   }
   
   for(i = 1; i < argc; i++)
   {
      fprintf(stderr, "Tokenising %s...\n", argv[i]);
      mcc_TokeniseFile(argv[i]);
      mcc_FreeTokens();
   }

   return 0;
} 
