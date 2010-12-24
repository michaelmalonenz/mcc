/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 */
#include <stdlib.h>

#include "mcc.h"


int main(int argc, char **argv)
{
   int i;
   
   if (argc <= 1)
   {
      mcc_Error("No input files\n");
   }
   
   for(i = 1; i < argc; i++)
   {
      mcc_PreprocessFile(argv[i], stdout);
   }

   return 0;
} 
