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

#define NUM_TEST_ITEMS 13

int test_data[NUM_TEST_ITEMS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

int main(void)
{
   int i;
   mcc_ListIterator_t *iter = NULL;
   mcc_List_t *list = mcc_ListCreate();

   printf("Beginning %s\n", __FILE__);
   printf("Setting up test data\n");
   
   for (i = 0; i < NUM_TEST_ITEMS; i++)
   {
      int *data = (int *) malloc(sizeof(int));
      *data = test_data[i];
      mcc_ListAppendData(list, data);
   }

   printf("Getting Iterator\n");
   iter = mcc_ListGetIterator(list);

   for (i = 0; i < NUM_TEST_ITEMS; i++)
   {
      int *result = (int *) mcc_ListGetNextData(iter);
      MCC_ASSERT(result != NULL);
      MCC_ASSERT(test_data[i] == *result);
   }

   printf("Cleaning up\n");
   mcc_ListDeleteIterator(iter);
   mcc_ListDelete(list);

   printf("Finished %s\n", __FILE__);

   return 0;
}
