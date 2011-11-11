/**
 * Mike's C Compiler
 *
 * Written and Directed by Michael Malone
 *
    mcc a lightweight compiler for developers, not machines
    Copyright (C) 2011 Michael Malone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#include <stdlib.h>

#include "mcc.h"
#include "tokenList.h"
#include "toolChainCommands.h"

int main(int argc, char **argv)
{
   int i;
   
   if (argc <= 1)
   {
      mcc_Error("No input files\n");
   }
   
   for(i = 1; i < argc; i++)
   {
      mcc_TokenListIterator_t *tokenListIter = mcc_GetTokenListIterator();
      fprintf(stderr, "Tokenising %s...\n", argv[i]);
      mcc_TokeniseFile(argv[i], tokenListIter);
      mcc_DeleteTokenListIterator(tokenListIter);
      fprintf(stderr, "Preprocessing %s...\n", argv[i]);
      mcc_PreprocessCurrentTokens();
      mcc_FreeTokens();
   }

   return 0;
} 
