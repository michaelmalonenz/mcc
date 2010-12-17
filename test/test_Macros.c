//void mcc_DefineMacro(char *text, char *value);

//void mcc_UndefineMacro(char *text);

//mcc_Macro_t *mcc_ResolveMacro(const char *text);
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "macro.h"

#define MACRO_NAME "EAT_ME"

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

int main(void)
{
   test_Define();
   test_Find();
   test_Undefine();
   return 0;
}
