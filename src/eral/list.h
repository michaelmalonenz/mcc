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
#ifndef _MCC_LIST_H_
#define _MCC_LIST_H_

#include <stdint.h>
#include <limits.h>

/* Forward declarations of the opaque list types */
typedef struct list_node mcc_ListNode_t;
typedef struct list mcc_List_t;
typedef struct iterator mcc_ListIterator_t;


typedef void (*mcc_NodeDestructor_fn)(uintptr_t);

#define NULL_DATA ((uintptr_t) 0)

/**
 * Initialises an empty unordered, doubly-linked list.
 * Any call to this must be matched with a call to mcc_ListDelete()
 */
mcc_List_t *mcc_ListCreate(void);

/**
 * Destroys the dynamic memory associated with both the list _AND_
 * all the items stored within it.
 * The supplied destructor function is for an individual data item in the list.
 */
void mcc_ListDelete(mcc_List_t *list, mcc_NodeDestructor_fn destructorFn);

/**
 * Adds an item to the end of the list
 */
void mcc_ListAppendData(mcc_List_t *list, uintptr_t data);

/**
 * Removes the item at the tail (most recently appended) of the list.
 * The item removed is no longer part of the list and responsibility
 * for freeing any memory associated with the item is transferred to the client
 */
uintptr_t mcc_ListRemoveTailData(mcc_List_t *list);

uintptr_t mcc_ListPeekTailData(mcc_List_t *list);

/**
 * Returns True if the list has no items in it, otherwise false
 */
bool_t mcc_ListEmpty(mcc_List_t *list);

uint32_t mcc_ListGetLength(mcc_List_t *list);

/**
 * Gets an iterator pointing nowhere in the list.
 * The next call to mcc_ListGetXXXXData determines whereabouts in the list we start.
 * If mcc_ListGetNextData() is called first, we start at the beginning (head),
 * conversely if mcc_ListGetPrevData() is called first, we start at the end (tail).
 * After this, any amount of Next or Prev may be called on any iterator, but if insertions
 * are made to the list (with another iterator) this may give unexpected results.
 * So long as there is nothing removed, the iterator is guaranteed to be valid,
 * but valid for the new version of the list.
 *
 * Any calls to this function should be matched with a call
 * to mcc_ListDeleteIterator()
 */
mcc_ListIterator_t *mcc_ListGetIterator(mcc_List_t *list);

/**
 * Destroys the dynamic memory associated with the iterator only.
 */
void mcc_ListDeleteIterator(mcc_ListIterator_t *iter);

/**
 * Copies an iterator for a list.  This means we can store whereabouts
 * in the list we are up to and continue to mcc_ListInsertDataAtCurrentPosition()
 * without breaking the original iterator.
 * Any call to this function should be matched with a call to mcc_ListDeleteIterator()
 */
mcc_ListIterator_t *mcc_ListCopyIterator(mcc_ListIterator_t *iter);

/**
 * Inserts a new item in the list just after the position where the current iterator
 * is pointing and updates the iterator to point at the new item, so a call to
 * mcc_ListGetNextData() will not return the new item, but the item which would
 * have been next in the list prior to a call to this function.
 */
void mcc_ListInsertDataAtCurrentPosition(mcc_ListIterator_t *iter, uintptr_t data);

/**
 * Remove the current data and return it.  Insert the list (data) in place of the old
 * current data.
 */
uintptr_t mcc_ListReplaceCurrentData(mcc_ListIterator_t *iter, mcc_List_t *data);

/**
 * This shows what data the iterator is currently pointing at.
 * It has no side-effects on the iterator and callers *must not*
 * free the memory associated with the data returned.
 *
 * NULL is returned if the iterator isn't pointing at anything.
 */
uintptr_t mcc_ListPeekCurrentData(mcc_ListIterator_t *iter);

/**
 * This shows what data would be returned if the function mcc_ListGetNextData
 * were called.  This does not alter the iterator position and callers
 * *must not* free the memory associated with the data returned.
 * 
 * NULL is returned if the iterator is not pointing at anything, or
 * the iterator is pointing at the tail of the list.
 */
uintptr_t mcc_ListPeekNextData(mcc_ListIterator_t *iter);

/**
 * This returns a copy of the head data. This differs from the
 * Peek Current Data, 
 */
uintptr_t mcc_ListPeekTailData(mcc_List_t *list);

/**
 * Returns the next item in the list or NULL if we are at the end of the list
 */
uintptr_t mcc_ListGetNextData(mcc_ListIterator_t *iter);

/**
 * Returns the previous item in the list or NULL if we are at the start of the list
 */
uintptr_t mcc_ListGetPrevData(mcc_ListIterator_t *iter);

/**
 * Concatenates the src list onto the dst list, freeing the memory
 * associated with the src list.
 */
void mcc_ListConcatenate(mcc_List_t *dst, mcc_List_t *src);

/**
 * Removes the data at the current position and returns it to the caller.
 * If we don't have any data in the list, or the iterator has no current
 * data, it returns NULL.
 * This is a destructive operation.
 */
uintptr_t mcc_ListRemoveCurrentData(mcc_ListIterator_t *iter);

#endif /* _MCC_LIST_H_ */
