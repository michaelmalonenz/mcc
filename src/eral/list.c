/**
 Copyright (c) 2012, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdlib.h>
#include <stdint.h>

#include "mcc.h"
#include "list.h"

struct list_node {
   uintptr_t data;
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

static mcc_ListNode_t *CreateListNode(uintptr_t data)
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
      current->data = NULL_DATA;
      free(current);
      list->nItems--;
      current = next;
   }

   MCC_ASSERT(list->nItems == 0);
   list->head = NULL;
   list->tail = NULL;

   free(list);
}

void mcc_ListAppendData(mcc_List_t *list, uintptr_t data)
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

uintptr_t mcc_ListRemoveTailData(mcc_List_t *list)
{
   if (list->tail != NULL)
   {
      mcc_ListNode_t *result_node = list->tail;
      uintptr_t result = list->tail->data;
      if (list->head == list->tail)
      {
         list->head = NULL;
         list->tail = NULL;
      }
      else
      {
         list->tail = list->tail->prev;
      }
      free(result_node);
      list->nItems--;
      return result;
   }
   return NULL_DATA;
}

uintptr_t mcc_ListPeekTailData(mcc_List_t *list)
{
   if (list->tail != NULL)
   {
      return list->tail->data;
   }
   return NULL_DATA;
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

void mcc_ListInsertDataAtCurrentPosition(mcc_ListIterator_t *iter, uintptr_t data)
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

uintptr_t mcc_ListReplaceCurrentData(mcc_ListIterator_t *iter, mcc_List_t *data)
{
   MCC_ASSERT(iter->current != NULL);
   uintptr_t previous = iter->current->data;
   data->tail->next = iter->current->next;
   if (iter->current->next != NULL)
      iter->current->next->prev = data->tail;
   
   data->head->prev = iter->current->prev;
   if (iter->current->prev != NULL)
      iter->current->prev->next = data->head;

   if (iter->current == iter->list->head)
      iter->list->head = data->head;

   if (iter->current == iter->list->tail)
      iter->list->tail = data->tail;

   data->head = NULL;
   data->tail = NULL;
   data->nItems = 0;

   iter->list->nItems += data->nItems;
   free(iter->current);
   iter->current = data->tail;
   return previous;
}

bool_t mcc_ListEmpty(mcc_List_t *list)
{
   return list->nItems == 0;
}

uint32_t mcc_ListGetLength(mcc_List_t *list)
{
   return list->nItems;
}

uintptr_t mcc_ListPeekCurrentData(mcc_ListIterator_t *iter)
{
   if (iter->current == NULL)
      return NULL_DATA;
   return iter->current->data;
}

uintptr_t mcc_ListPeekNextData(mcc_ListIterator_t *iter)
{
   if (iter->current == NULL || iter->current->next == NULL)
      return NULL_DATA;
   return iter->current->next->data;
}

uintptr_t mcc_ListGetNextData(mcc_ListIterator_t *iter)
{
   if (iter->current == iter->list->tail)
   {
      iter->current = NULL;
      return NULL_DATA;
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

uintptr_t mcc_ListGetPrevData(mcc_ListIterator_t *iter)
{
   if (iter->current == iter->list->head)
   {
      iter->current = NULL;
      return NULL_DATA;
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

