#ifndef _MCC_LIST_H_
#define _MCC_LIST_H_

/* Forward declarations of the opaque list types */
typedef struct list_node mcc_ListNode_t;
typedef struct list mcc_List_t;
typedef struct iterator mcc_ListIterator_t;

/**
 * Initialises an empty unordered, singly-linked list.
 * Any call to this must be matched with a call to mcc_ListDelete()
 * As it is singly-linked, this should only be a forward-referencing list.
 * Iterators will not pick up changes if items are added behind their current position.
 */
mcc_List_t *mcc_ListCreate(void);

/**
 * Destroys the dynamic memory associated with both the list _AND_
 * all the items stored within it.
 */
void mcc_ListDelete(mcc_List_t *list);

/**
 * Adds an item to the end of the list
 */
void mcc_ListAppendData(mcc_List_t *list, void *data);

/**
 * Gets an iterator pointing to the beginning of the list.
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
 * Returns the next item in the list or NULL if we are at the end of the list
 */
void *mcc_ListGetNextData(mcc_ListIterator_t *iter);


#endif /* _MCC_LIST_H_ */
