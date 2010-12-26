#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
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
   assert(result != NULL);
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
}

int main(void)
{
   test_Define();
   test_Find();
   test_Undefine();
   test_BulkMacros();
   return 0;
}
