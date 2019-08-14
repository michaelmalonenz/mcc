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
#include "liberal.h"
#include "list.h"

struct list_node {
   uintptr_t data;
   struct list_node *next;
   struct list_node *prev;
};

struct list {
   eral_ListNode_t *head;
   eral_ListNode_t *tail;
   uint32_t nItems;
};

struct iterator {
   eral_ListNode_t *current;
   eral_List_t *list;
};

static eral_ListNode_t *CreateListNode(uintptr_t data)
{
   eral_ListNode_t *node = (eral_ListNode_t *) malloc(sizeof(eral_ListNode_t));
   MCC_ASSERT(node != NULL);
   node->next = NULL;
   node->prev = NULL;
   node->data = data;
   return node;
}

eral_List_t *eral_ListCreate(void)
{
   eral_List_t *result = (eral_List_t *) malloc(sizeof(eral_List_t));
   MCC_ASSERT(result != NULL);
   result->head = NULL;
   result->tail = NULL;
   result->nItems = 0;
   return result;
}

void eral_ListDelete(eral_List_t *list, eral_NodeDestructor_fn destructorFn)
{
   eral_ListNode_t *current = NULL;
   eral_ListNode_t *next = NULL;

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

void eral_ListAppendData(eral_List_t *list, uintptr_t data)
{
   eral_ListNode_t *node = CreateListNode(data);

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
   list->nItems++;
}

uintptr_t eral_ListRemoveTailData(eral_List_t *list)
{
   if (list->tail != NULL)
   {
      eral_ListNode_t *result_node = list->tail;
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

uintptr_t eral_ListPeekTailData(eral_List_t *list)
{
   if (list->tail != NULL)
   {
      return list->tail->data;
   }
   return NULL_DATA;
}

eral_ListIterator_t *eral_ListGetIterator(eral_List_t *list)
{
   eral_ListIterator_t *iter = (eral_ListIterator_t *) malloc(sizeof(eral_ListIterator_t));
   MCC_ASSERT(iter != NULL);
   MCC_ASSERT(list != NULL);
   iter->list = list;
   iter->current = NULL;
   return iter;
}

eral_ListIterator_t *eral_ListCopyIterator(eral_ListIterator_t *iter)
{
   eral_ListIterator_t *result = (eral_ListIterator_t *) malloc(sizeof(eral_ListIterator_t));
   MCC_ASSERT(iter != NULL);
   MCC_ASSERT(result != NULL);
   result->list = iter->list;
   result->current = iter->current;
   return result;
}

void eral_ListDeleteIterator(eral_ListIterator_t *iter)
{
   MCC_ASSERT(iter != NULL);
   /* These two aren't strictly necessary, but if anyone tries to use
    * the iterator after it's been free'd it should segfault :)
    */
   iter->current = NULL;
   iter->list = NULL;
   free(iter);
}

void eral_ListInsertDataAtCurrentPosition(eral_ListIterator_t *iter, uintptr_t data)
{
   eral_ListNode_t *node = CreateListNode(data);
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

uintptr_t eral_ListReplaceCurrentData(eral_ListIterator_t *iter, eral_List_t *data)
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

   iter->list->nItems += data->nItems - 1;

   data->head = NULL;
   data->tail = NULL;
   data->nItems = 0;

   free(iter->current);
   iter->current = data->tail;

   return previous;
}

bool_t eral_ListEmpty(eral_List_t *list)
{
   return list->nItems == 0;
}

uint32_t eral_ListGetLength(eral_List_t *list)
{
   return list->nItems;
}

uint32_t eral_ListIteratorGetLength(eral_ListIterator_t *iter)
{
   return eral_ListGetLength(iter->list);
}

uintptr_t eral_ListPeekCurrentData(eral_ListIterator_t *iter)
{
   if (iter->current == NULL)
      return NULL_DATA;
   return iter->current->data;
}

uintptr_t eral_ListPeekNextData(eral_ListIterator_t *iter)
{
   if (iter->current == NULL || iter->current->next == NULL)
      return NULL_DATA;
   return iter->current->next->data;
}

uintptr_t eral_ListGetNextData(eral_ListIterator_t *iter)
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

uintptr_t eral_ListGetPrevData(eral_ListIterator_t *iter)
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

void eral_ListConcatenate(eral_List_t *dst, eral_List_t *src)
{
   eral_ListIterator_t *iter = eral_ListGetIterator(src);
   uintptr_t current = eral_ListGetNextData(iter);
   while (current != NULL_DATA)
   {
      eral_ListAppendData(dst, current);
      current = eral_ListGetNextData(iter);
   }
   eral_ListDeleteIterator(iter);
   eral_ListDelete(src, NULL);
}

uintptr_t eral_ListRemoveCurrentData(eral_ListIterator_t *iter)
{
   eral_ListNode_t *current = iter->current;
   if (current == NULL)
      return NULL_DATA;

   if (current->prev != NULL)
   {
      current->prev->next = current->next;
   }
   else
   {
      iter->list->head = current->next;
   }

   if (current->next != NULL)
   {
      current->next->prev = current->prev;
   }
   else
   {
      iter->list->tail = current->prev;
   }
   iter->current = iter->current->prev;
   iter->list->nItems--;
   uintptr_t result = (uintptr_t) current->data;
   free(current);
   return (uintptr_t)result;
}