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
#ifndef _MCC_LIST_H_
#define _MCC_LIST_H_

/* Forward declarations of the opaque list types */
typedef struct list_node mcc_ListNode_t;
typedef struct list mcc_List_t;
typedef struct iterator mcc_ListIterator_t;


typedef void (*mcc_NodeDestructor_fn)(void *);

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
void mcc_ListAppendData(mcc_List_t *list, void *data);

/**
 * Gets an iterator pointing nowhere in the list.
 * The next call to mcc_ListGetXXXXData determines whereabouts in the list we start.
 * If mcc_ListGetNextData() is called first, we start at the beginning,
 * conversely if mcc_ListGetPrevData() is called first, we start at the end.
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
void mcc_ListInsertDataAtCurrentPosition(mcc_ListIterator_t *iter, void *data);

/**
 * Removes the item currently pointed at by the iterator from the list and returns it.
 * If the iterator isn't pointing to anything in the list, then NULL is returned.
 *  i.e. mcc_ListGetXXXXData has to have been called at least once in order to set the
 *       iterator to be pointing at something.
 * The caller of this function becomes responsible for freeing the memory associated
 * with the return data, if it is necessary to do so.
 *
 * After removing the item currently pointed to by the iterator, the iterator will be
 * moved back one position, such that calls to mcc_ListGetNextData() will still return
 * the same value as prior to the remove, however, mcc_ListGetPrevData() will "skip" one
 * and return the item that would have been returned on the _second_ consecutive call 
 * to mcc_ListGetPrevData().
 *
 * This function potentially invalidates other iterators (not necessarily under your 
 * immediate control).  Use this function with care.
 */
void *mcc_ListRemoveDataAtCurrentPosition(mcc_ListIterator_t *iter);

/**
 * Returns the next item in the list or NULL if we are at the end of the list
 */
void *mcc_ListGetNextData(mcc_ListIterator_t *iter);

/**
 * Returns the previous item in the list or NULL if we are at the start of the list
 */
void *mcc_ListGetPrevData(mcc_ListIterator_t *iter);

#endif /* _MCC_LIST_H_ */
