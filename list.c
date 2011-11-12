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
#include <stdlib.h>
#include <stdint.h>

#include "mcc.h"
#include "list.h"

struct list_node {
   void *data;
   struct list_node *next;
   struct list_node *prev;
};

struct list {
   mcc_ListNode_t *head;
   mcc_ListNode_t *tail;
   uint32_t nItems;
};

struct iterator {
   mcc_ListNode_t *current;
   mcc_List_t *list;
};

static mcc_ListNode_t *CreateListNode(void *data)
{
   mcc_ListNode_t *node = (mcc_ListNode_t *) malloc(sizeof(mcc_ListNode_t));
   MCC_ASSERT(node != NULL);
   node->next = NULL;
   node->prev = NULL;
   node->data = data;
   return node;
}

mcc_List_t *mcc_ListCreate(void)
{
   mcc_List_t *result = (mcc_List_t *) malloc(sizeof(mcc_List_t));
   MCC_ASSERT(result != NULL);
   result->head = NULL;
   result->tail = NULL;
   result->nItems = 0;
   return result;
}

void mcc_ListDelete(mcc_List_t *list, mcc_NodeDestructor_fn destructorFn)
{
   mcc_ListNode_t *current = NULL;
   mcc_ListNode_t *next = NULL;

   current = list->head;

   while (current != NULL)
   {
      next = current->next;
      if (destructorFn != NULL)
      {
         destructorFn(current->data);
      }
      current->data = NULL;
      free(current);
      list->nItems--;
      current = next;
   }

   MCC_ASSERT(list->nItems == 0);
   list->head = NULL;
   list->tail = NULL;

   free(list);
}

void mcc_ListAppendData(mcc_List_t *list, void *data)
{
   mcc_ListNode_t *node = CreateListNode(data);

   if (list->head == NULL)
   {
      list->head = node;
      list->tail = node;
   }
   else
   {
      list->tail->next = node;
      node->prev = list->tail;
      list->tail = node;
   }
   list->nItems ++;
}

mcc_ListIterator_t *mcc_ListGetIterator(mcc_List_t *list)
{
   mcc_ListIterator_t *iter = (mcc_ListIterator_t *) malloc(sizeof(mcc_ListIterator_t));
   MCC_ASSERT(iter != NULL);
   MCC_ASSERT(list != NULL);
   iter->list = list;
   iter->current = NULL;
   return iter;
}

mcc_ListIterator_t *mcc_ListCopyIterator(mcc_ListIterator_t *iter)
{
   mcc_ListIterator_t *result = (mcc_ListIterator_t *) malloc(sizeof(mcc_ListIterator_t));
   MCC_ASSERT(iter != NULL);
   MCC_ASSERT(result != NULL);
   result->list = iter->list;
   result->current = iter->current;
   return result;
}

void mcc_ListDeleteIterator(mcc_ListIterator_t *iter)
{
   MCC_ASSERT(iter != NULL);
   /* These two aren't strictly necessary, but if anyone tries to use
    * the iterator after it's been free'd it should segfault :)
    */
   iter->current = NULL;
   iter->list = NULL;
   free(iter);
}

void mcc_ListInsertDataAtCurrentPosition(mcc_ListIterator_t *iter, void *data)
{
   mcc_ListNode_t *node = CreateListNode(data);
   if (iter->current == NULL)
   {
      if (iter->list->head == NULL)
      {
         iter->list->head = node;
         iter->list->tail = node;
      }
      else
      {
         node->next = iter->list->head;
         node->next->prev = node;
         iter->list->head = node;
      }
   }
   else
   {
      node->next = iter->current->next;
      if (node->next != NULL)
      {
         node->next->prev = node;
      }
      node->prev = iter->current;
      iter->current->next = node;
      if (iter->current == iter->list->tail)
      {
         iter->list->tail = node;
      }
   }
   iter->current = node;
   iter->list->nItems++;
}

void *mcc_ListGetNextData(mcc_ListIterator_t *iter)
{
   if (iter->current == iter->list->tail)
   {
      iter->current = NULL;
      return NULL;
   }
   
   if (iter->current == NULL)
   {
      iter->current = iter->list->head;
   }
   else
   {
      iter->current = iter->current->next;
   }
   return iter->current->data;
}

void *mcc_ListGetPrevData(mcc_ListIterator_t *iter)
{
   if (iter->current == iter->list->head)
   {
      iter->current = NULL;
      return NULL;
   }
   
   if (iter->current == NULL)
   {
      iter->current = iter->list->tail;
   }
   else
   {
      iter->current = iter->current->prev;
   }
   return iter->current->data;
}
