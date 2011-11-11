/**
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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define MCC_DEBUG 1
#include "mcc.h"
#include "macro.h"

#define MACRO_NAME "EAT_ME"

#define NUM_BULK_MACROS 10
const char *test_Macros[NUM_BULK_MACROS] = { "FIRST_MACRO", "SECOND_MACRO", "THIRD_MACRO", 
                                             "FOURTH_MACRO", "FIFTH_MACRO", "SIXTH_MACRO",
                                             "SEVENTH_MACRO", "EIGHTH_MACRO", "NINETH_MACRO",
                                             "TENTH_MACRO" };

char *test_MacroValues[NUM_BULK_MACROS] = { "1", "two", "1+2", "(FIRST_MACRO) + 3", "5", "2*3",
                                                  "7", "8", "(2*4+1)", "1+2*3+3" };

static void test_Define(void)
{
   printf("Testing Define...");
   mcc_DefineMacro(MACRO_NAME, "(1)");
   printf("ok\n");
}

static void test_Find(void)
{	
   printf("Testing Find...");
   mcc_Macro_t *result = mcc_ResolveMacro(MACRO_NAME);
   MCC_ASSERT(result != NULL);
   printf("ok\n");
}


static void test_Undefine(void)
{
   printf("Testing undefine...");
   mcc_UndefineMacro(MACRO_NAME);
   printf("ok\n");
}


static void test_BulkMacros(void)
{
   int i;
   printf("Testing Bulk Macro Definitions...");
   for(i = 0; i < NUM_BULK_MACROS; i++)
   {
      mcc_DefineMacro(test_Macros[i], test_MacroValues[i]);
   }
   mcc_DeleteAllMacros();
   printf("ok\n");
}

int main(void)
{
   test_Define();
   test_Find();
   test_Undefine();
   test_BulkMacros();
   return 0;
}
