/*
 * Copyright 2012 Steven Gribble
 *
 *  This file is part of the UW CSE 333 project sequence (333proj12).
 *
 *  333proj12 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  333proj12 is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj12.  If not, see <http://www.gnu.org/licenses/>.
 */

// Tobias Kahan
// 0829117
// tobyk100@uw.edu

#include <stdio.h>
#include <stdlib.h>

#include "Assert333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"

// Validates that the the iterator, the iterators list, and the iterators
// current node are all nonnull.
void ValidateIter(LLIter iter);

// Pops (slices) a list with zero or one nodes. These two operations
// are the same for a zero or one element list so we combine the functionality
// here. The list must have zero or one element.
// Beside popping/slicing the node this function has the side effect of setting 
// payload_ptr and updating the list size.
bool PopSliceLessThanTwo(LinkedList, void **payload_ptr);

LinkedList AllocateLinkedList(void) {
  // allocate the linked list record
  LinkedList ll = (LinkedList) malloc(sizeof(LinkedListHead));
  if (ll == NULL) {
    // out of memory
    return (LinkedList) NULL;
  }

  ll->num_elements = 0U;
  ll->head = ll->tail = NULL;

  // return our newly minted linked list
  return ll;
}

void FreeLinkedList(LinkedList list,
                    LLPayloadFreeFnPtr payload_free_function) {
  // defensive programming
  Assert333(list != NULL);
  Assert333(payload_free_function != NULL);

  while (list->head != NULL) {
    payload_free_function(list->head->payload);
    LinkedListNodePtr oldHead = list->head;
    list->head = list->head->next;
    free(oldHead);
  }

  // free the list record
  free(list);
}

uint64_t NumElementsInLinkedList(LinkedList list) {
  // defensive programming
  Assert333(list != NULL);
  return list->num_elements;
}

bool PushLinkedList(LinkedList list, void *payload) {
  // defensive programming
  Assert333(list != NULL);

  // allocate space for the new node.
  LinkedListNodePtr ln =
    (LinkedListNodePtr) malloc(sizeof(LinkedListNode));
  if (ln == NULL) {
    // out of memory
    return false;
  }

  ln->payload = payload;  // set the payload
  ln->prev = NULL;  // ln becomes new head so set prev to NULL
  if (list->num_elements == 0) {
    // degenerate case; list is currently empty
    ln->next = NULL;
    list->tail = ln;
  } else {
    // typical case; list has >=1 elements
    ln->next = list->head;  // set ln to point to current head
    list->head->prev = ln;
  }
  list->head = ln;
  list->num_elements++;

  return true;
}

bool PopLinkedList(LinkedList list, void **payload_ptr) {
  // defensive programming.
  Assert333(payload_ptr != NULL);
  Assert333(list != NULL);

  if (list->num_elements < 2) {
    return PopSliceLessThanTwo(list, payload_ptr);
  }
  // typical case; list has >= 1 elements
  *payload_ptr = list->head->payload;
  LinkedListNodePtr oldHead = list->head;
  list->head = oldHead->next;
  free(oldHead);
  list->head->prev = NULL;
  list->num_elements--;

  return true;
}

bool AppendLinkedList(LinkedList list, void *payload) {
  // defensive programming.
  Assert333(list != NULL);

  if (list->num_elements == 0) {
    // degenerate case; list is currently empty
    return PushLinkedList(list, payload);
  }

  // allocate space for the new node.
  LinkedListNodePtr ln = (LinkedListNodePtr) malloc(sizeof(LinkedListNode));
  if (ln == NULL) {
    // out of memory
    return false;
  }

  // set the payload
  ln->payload = payload;
  // typical case; list has >= 1 node.
  list->tail->next = ln;
  ln->prev = list->tail;
  ln->next = NULL;
  list->tail = ln;
  list->num_elements++;

  return true;
}

bool SliceLinkedList(LinkedList list, void **payload_ptr) {
  // defensive programming.
  Assert333(payload_ptr != NULL);
  Assert333(list != NULL);
  
  if (list->num_elements < 2) {
    return PopSliceLessThanTwo(list, payload_ptr);
  }
  // typical case; list has >= 1 elements
  *payload_ptr = list->tail->payload;
  LinkedListNodePtr oldTail = list->tail;
  list->tail = list->tail->prev;
  free(oldTail);
  list->tail->next = NULL;
  list->num_elements--;
  return true;
}

void SortLinkedList(LinkedList list, unsigned int ascending,
                    LLPayloadComparatorFnPtr comparator_function) {
  Assert333(list != NULL);  // defensive programming
  if (list->num_elements < 2) {
    // no sorting needed
    return;
  }

  // we'll implement bubblesort! nice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNodePtr curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // bubble-swap payloads
        void *tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}

LLIter LLMakeIterator(LinkedList list, int pos) {
  // defensive programming
  Assert333(list != NULL);
  Assert333((pos == 0) || (pos == 1));

  // if the list is empty, return failure.
  if (NumElementsInLinkedList(list) == 0U)
    return NULL;

  // OK, let's manufacture an iterator.
  LLIter li = (LLIter) malloc(sizeof(LLIterSt));
  if (li == NULL) {
    // out of memory!
    return NULL;
  }

  // set up the iterator.
  li->list = list;
  if (pos == 0) {
    li->node = list->head;
  } else {
    li->node = list->tail;
  }

  // return the new iterator
  return li;
}

void LLIteratorFree(LLIter iter) {
  // defensive programming
  Assert333(iter != NULL);
  free(iter);
}

bool LLIteratorHasNext(LLIter iter) {
  // defensive programming
  ValidateIter(iter);

  // Is there another node beyond the iterator?
  if (iter->node->next == NULL) {
    return false;  // no
  }

  return true;  // yes
}

bool LLIteratorNext(LLIter iter) {
  // defensive programming
  ValidateIter(iter);

  if (LLIteratorHasNext(iter)) {
    iter->node = iter->node->next;
    return true;
  }

  // Nope, there isn't another node, so return failure.
  return false;
}

bool LLIteratorHasPrev(LLIter iter) {
  // defensive programming
  ValidateIter(iter);

  // Is there another node beyond the iterator?
  if (iter->node->prev == NULL)
    return false;  // no

  return true;  // yes
}

bool LLIteratorPrev(LLIter iter) {
  // defensive programming
  ValidateIter(iter);

  if (LLIteratorHasPrev(iter)) {
    iter->node = iter->node->prev;
    return true;
  }

  // nope, so return failure.
  return false;
}

void LLIteratorGetPayload(LLIter iter, void **payload) {
  // defensive programming
  ValidateIter(iter);

  // set the return parameter.
  *payload = iter->node->payload;
}

bool LLIteratorDelete(LLIter iter,
                      LLPayloadFreeFnPtr payload_free_function) {
  // defensive programming
  ValidateIter(iter);
  
  void *payload_ptr = NULL;
  if (iter->list->num_elements == 1) {
    // degenerative case: only one element.
    PopSliceLessThanTwo(iter->list, &payload_ptr);
    payload_free_function(payload_ptr);
    iter->node = NULL;
    return false;
  } else if (iter->node == iter->list->head) {
    // degenerative case: iterator's current node is the head.
    LinkedListNodePtr newNode = iter->node->next;
    PopLinkedList(iter->list, &payload_ptr);
    iter->node = newNode;
  } else if (iter->node == iter->list->tail) {
    // degenerative case: iterator's current node is the tail.
    LinkedListNodePtr newNode= iter->list->tail->prev;
    SliceLinkedList(iter->list, &payload_ptr);
    iter->node = newNode;
  } else {
    // typical case: at least 3 elements and iterator points to a middle node.
    LinkedListNodePtr deletedNode = iter->node;
    deletedNode->prev->next = deletedNode->next;
    deletedNode->next->prev = deletedNode->prev;
    iter->node = deletedNode->next;
    payload_ptr = deletedNode->payload;
    free(deletedNode);
    iter->list->num_elements--;
  }
  payload_free_function(payload_ptr);
  return true;
}

bool LLIteratorInsertBefore(LLIter iter, void *payload) {
  // defensive programming.
  ValidateIter(iter);

  // If the cursor is pointing at the head, use our
  // PushLinkedList function.
  if (iter->node == iter->list->head) {
    return PushLinkedList(iter->list, payload);
  }

  // General case: we have to do some splicing.
  LinkedListNodePtr newnode =
    (LinkedListNodePtr) malloc(sizeof(LinkedListNode));
  if (newnode == NULL)
    return false;  // out of memory

  newnode->payload = payload;
  newnode->next = iter->node;
  newnode->prev = iter->node->prev;
  newnode->prev->next = newnode;
  newnode->next->prev = newnode;
  iter->list->num_elements += 1;
  return true;
}

void ValidateIter(LLIter iter) {
  Assert333(iter != NULL);
  Assert333(iter->list != NULL);
  Assert333(iter->node != NULL);
}

bool PopSliceLessThanTwo(LinkedList list, void **payload_ptr) {
  // defensive programming.
  Assert333(list->num_elements <= 1);

  if (list->num_elements == 0) {
    return false;
  }
  *payload_ptr = list->head->payload;
  free(list->head);
  list->head = list->tail = NULL;
  list->num_elements--;
  return true;
}
