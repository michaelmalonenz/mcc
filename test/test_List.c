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
#include <stdio.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"
#include "list.h"

#define NUM_BASIC_TEST_ITEMS 10
#define NUM_ITEMS_TO_INSERT 6

int basic_test_data[NUM_BASIC_TEST_ITEMS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

int insertion_test_data[NUM_ITEMS_TO_INSERT] = { 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };

static void test_BasicListFunctionality(void)
{
   int i;
   mcc_ListIterator_t *iter = NULL;
   mcc_List_t *list = mcc_ListCreate();

   printf("Setting up basic test data\n");
   
   for (i = 0; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      int *data = (int *) malloc(sizeof(int));
      *data = basic_test_data[i];
      mcc_ListAppendData(list, data);
   }

   printf("Getting Iterator\n");
   iter = mcc_ListGetIterator(list);

   for (i = 0; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      int *result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }

   printf("Cleaning up basic test\n");
   mcc_ListDeleteIterator(iter);
   mcc_ListDelete(list);
   printf("Finished basic test\n");
}

static void test_NullList(void)
{
   mcc_ListIterator_t *iter = NULL;
   mcc_List_t *list = mcc_ListCreate();

   printf("Starting Null test\n");

   printf("Getting Iterator\n");
   iter = mcc_ListGetIterator(list);

   MCC_ASSERT(mcc_ListGetNextData(iter) == NULL);

   printf("Cleaning up Null test\n");
   mcc_ListDeleteIterator(iter);
   mcc_ListDelete(list);

   printf("Finished Null test\n");
}

static void test_InsertionWithIterator(void)
{
   int i;
   mcc_ListIterator_t *iter = NULL;
   mcc_List_t *list = mcc_ListCreate();

   printf("Starting insertion test\n");
   for (i = 0; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      int *data = (int *) malloc(sizeof(int));
      *data = basic_test_data[i];
      mcc_ListAppendData(list, data);
   }

   printf("Getting Iterator\n");
   iter = mcc_ListGetIterator(list);

   for (i = 0; i < (NUM_BASIC_TEST_ITEMS/2); i++)
   {
      int *result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }

   for (i = 0; i < NUM_ITEMS_TO_INSERT; i++)
   {
      int *data = (int *) malloc(sizeof(int));
      *data = insertion_test_data[i];
      mcc_ListInsertDataAtCurrentPosition(iter, data);
   }

   for (i = (NUM_BASIC_TEST_ITEMS/2); i < NUM_BASIC_TEST_ITEMS; i++)
   {
      int *result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }

   printf("Cleaning up insertion test\n");
   mcc_ListDeleteIterator(iter);
   mcc_ListDelete(list);

   printf("Finished insertion test\n");
}

static void test_CopyingIterator(void)
{
   int i;
   mcc_ListIterator_t *iter = NULL;
   mcc_ListIterator_t *iter_copy = NULL;
   mcc_List_t *list = mcc_ListCreate();
   int *result = NULL;

   printf("Setting up basic iterator test data\n");
   
   for (i = 0; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      int *data = (int *) malloc(sizeof(int));
      *data = basic_test_data[i];
      mcc_ListAppendData(list, data);
   }

   printf("Getting initial Iterator\n");
   iter = mcc_ListGetIterator(list);

   for (i = 0; i < NUM_BASIC_TEST_ITEMS/2; i++)
   {
      result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }

   printf("Copying Iterator\n");
   iter_copy = mcc_ListCopyIterator(iter);

   for (i = NUM_BASIC_TEST_ITEMS/2; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }
   result = (int *) mcc_ListGetNextData(iter);
   MCC_ASSERT(result == NULL);

   for (i = NUM_BASIC_TEST_ITEMS/2; i < NUM_BASIC_TEST_ITEMS; i++)
   {
      result = (int *) mcc_ListGetNextData(iter_copy);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(basic_test_data[i] == *result);
   }
   result = (int *) mcc_ListGetNextData(iter_copy);
   MCC_ASSERT(result == NULL);

   printf("Cleaning up basic iterator test\n");
   mcc_ListDeleteIterator(iter);
   mcc_ListDeleteIterator(iter_copy);
   mcc_ListDelete(list);
   printf("Finished basic iterator test\n");
}

int main(void)
{
   printf("Beginning %s\n", __FILE__);

   test_NullList();

   test_BasicListFunctionality();

   test_InsertionWithIterator();

   test_CopyingIterator();

   printf("Finished %s\n", __FILE__);
   return 0;
}
