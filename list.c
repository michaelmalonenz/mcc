#include <stdlib.h>

#include "mcc.h"
#include "list.h"

struct list_node {
   void *data;
   struct list_node *next;
};

struct list {
   mcc_ListNode_t *head;
   mcc_ListNode_t *tail;
};

struct iterator {
   mcc_ListNode_t *current;
   mcc_List_t *list;
};


mcc_List_t *mcc_ListCreate(void)
{
   mcc_List_t *result = (mcc_List_t *) malloc(sizeof(mcc_List_t));
   MCC_ASSERT(result != NULL);
   result->head = NULL;
   result->tail = NULL;
   return result;
}

void mcc_ListDelete(mcc_List_t UNUSED(*list))
{
   
}

void mcc_ListAppendData(mcc_List_t UNUSED(*list), void UNUSED(*data)) {}

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

void mcc_ListInsertDataAtCurrentPosition(mcc_ListIterator_t UNUSED(*iter), void UNUSED(*data)) {}

void *mcc_ListGetNext(mcc_ListIterator_t *iter)
{
   if (iter->current == iter->list->tail)
   {
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
