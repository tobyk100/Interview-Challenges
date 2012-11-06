/*
 * Copyright 2011 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

// Tobias Kahan
// 0829117
// tobyk100@uw.edu

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Assert333.h"
#include "HashTable.h"
#include "HashTable_priv.h"

// A private utility function to grow the hashtable (increase
// the number of buckets) if its load factor has become too high.
static void ResizeHashtable(HashTable ht);

// a free function that does nothing
static void NullFree(void *freeme) { }

// A private utility function that combines the actions of looking up and removing.
int LookupRemoveHashTable(HashTable table, uint64_t key, HTKeyValue *keyvalue, 
                          bool remove);
HashTable AllocateHashTable(uint32_t num_buckets) {
  HashTable ht;
  uint32_t  i;

  // defensive programming
  if (num_buckets == 0) {
    return NULL;
  }

  // allocate the hash table record
  ht = (HashTable) malloc(sizeof(HashTableRecord));
  if (ht == NULL) {
    return NULL;
  }

  // initialize the record
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets =
    (LinkedList *) malloc(num_buckets * sizeof(LinkedList));
  if (ht->buckets == NULL) {
    // make sure we don't leak!
    free(ht);
    return NULL;
  }
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = AllocateLinkedList();
    if (ht->buckets[i] == NULL) {
      // allocating one of our bucket chain lists failed,
      // so we need to free everything we allocated so far
      // before returning NULL to indicate failure.  Since
      // we know the chains are empty, we'll pass in a
      // free function pointer that does nothing; it should
      // never be called.
      uint32_t j;
      for (j = 0; j < i; j++) {
        FreeLinkedList(ht->buckets[j], NullFree);
      }
      free(ht);
      return NULL;
    }
  }

  return (HashTable) ht;
}

void FreeHashTable(HashTable table,
                   ValueFreeFnPtr value_free_function) {
  uint32_t i;

  // defensive programming.
  Assert333(table != NULL);

  // loops through and frees the chains on each bucket
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList  bl = table->buckets[i];
    HTKeyValue *nextKV;

    // pops elements off the the chain list, then frees the list
    while (NumElementsInLinkedList(bl) > 0) {
      Assert333(PopLinkedList(bl, (void **) &nextKV));
      value_free_function(nextKV->value);
      free(nextKV);
    }
    // the chain list is empty, so we can pass in the
    // null free function to FreeLinkedList.
    FreeLinkedList(bl, NullFree);
  }

  // free the bucket array within the table record,
  // then free the table record itself.
  free(table->buckets);
  free(table);
}

uint64_t NumElementsInHashTable(HashTable table) {
  Assert333(table != NULL);
  return table->num_elements;
}

uint64_t FNVHash64(unsigned char *buffer, unsigned int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //
  // http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  /*
   * FNV-1a hash each octet of the buffer
   */
  while (bp < be) {
    /* xor the bottom with the current octet */
    hval ^= (uint64_t) * bp++;
    /* multiply by the 64 bit FNV magic prime mod 2^64 */
    hval *= FNV_64_PRIME;
  }
  /* return our new hash value */
  return hval;
}

uint64_t FNVHashInt64(uint64_t hashme) {
  unsigned char buf[8];
  int i;

  for (i = 0; i < 8; i++) {
    buf[i] = (unsigned char) (hashme & 0x00000000000000FFULL);
    hashme >>= 8;
  }
  return FNVHash64(buf, 8);
}

uint64_t HashKeyToBucketNum(HashTable table, uint64_t key) {
  return key % table->num_buckets;
}

LinkedList GetChainByKey(HashTable table, uint64_t key) {
  uint32_t insertbucket = HashKeyToBucketNum(table, key);
  return table->buckets[insertbucket];  
}


int InsertHashTable(HashTable table,
                    HTKeyValue newkeyvalue,
                    HTKeyValue *oldkeyvalue) {
  // Defensive Programming.
  Assert333(table != NULL);
  Assert333(oldkeyvalue != NULL);
  ResizeHashtable(table);

  LinkedList chain = GetChainByKey(table, newkeyvalue.key);
  Assert333(chain != NULL);

  int keyAlreadyInChain = 0;
  HTKeyValuePtr newKVPtr = (HTKeyValuePtr) malloc(sizeof(newkeyvalue));
  if (newKVPtr == NULL) {
    return 0;  // Memory error, fail.
  }
  *newKVPtr = newkeyvalue;  // Copy contents of newkeyvalue newKVPtr heap block
  switch (LookupRemoveHashTable(table, newkeyvalue.key, oldkeyvalue, true)) {
    case -1:  // There was an error with the lookup, free memory and exit.
      free(newKVPtr);
      return 0;
    case 0: // The key was not found in the HashTable.
      break;
    case 1: // They key was already in the HashTable.
      keyAlreadyInChain = 1;
      break;
  }
  if (AppendLinkedList(chain, newKVPtr)) {
    table->num_elements++;
    return keyAlreadyInChain + 1;
  } else {  // AppendLinkedList failed, free memory and exit.
    free(newKVPtr);
    return 0;
  }
}



int LookupHashTable(HashTable table,
                    uint64_t key,
                    HTKeyValue *keyvalue) {
  Assert333(table != NULL);
  Assert333(keyvalue != NULL);

  return LookupRemoveHashTable(table, key, keyvalue, false);
}

int RemoveFromHashTable(HashTable table,
                        uint64_t key,
                        HTKeyValue *keyvalue) {
  Assert333(table != NULL);
  Assert333(keyvalue != NULL);

  return LookupRemoveHashTable(table, key, keyvalue, true);
}


HTIter HashTableMakeIterator(HashTable table) {
  HTIterRecord *iter;
  uint32_t      i;

  Assert333(table != NULL);  // be defensive

  // malloc the iterator
  iter = (HTIterRecord *) malloc(sizeof(HTIterRecord));
  if (iter == NULL) {
    return NULL;
  }

  // if the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->is_valid = false;
    iter->ht = table;
    iter->bucket_it = NULL;
    return iter;
  }

  // initialize the iterator.  there is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->is_valid = true;
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (NumElementsInLinkedList(table->buckets[i]) > 0) {
      iter->bucket_num = i;
      break;
    }
  }
  Assert333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLMakeIterator(table->buckets[iter->bucket_num], 0UL);
  if (iter->bucket_it == NULL) {
    // out of memory!
    free(iter);
    return NULL;
  }
  return iter;
}

void HTIteratorFree(HTIter iter) {
  Assert333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIteratorFree(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  iter->is_valid = false;
  free(iter);
}

//int HTIteratorNextNonemptyBucket(HTIter int currentBucket) {



int HTIteratorNext(HTIter iter) {
  Assert333(iter != NULL);

  uint32_t i;
  if (!iter->is_valid) {
    return 0;
  }

  if (!LLIteratorNext(iter->bucket_it)) {  // At the end of this bucket/list
    LLIteratorFree(iter->bucket_it);
    iter->bucket_it = NULL;
    for (i = iter->bucket_num + 1; i < iter->ht->num_buckets; i++) {
      if (NumElementsInLinkedList(iter->ht->buckets[i]) > 0) {
          iter->bucket_num = i;
          break;
      }
    }
    if (i >= iter->ht->num_buckets) {  
      // For loop did not find a nonempty bucket before hitting end of table.
      iter->is_valid = false;
      return 0;
    } 
    // For loop found a nonempty bucket and iter->bucket_num is set.
    iter->bucket_it = LLMakeIterator(iter->ht->buckets[iter->bucket_num], 0UL);
  }
  // Either we skipped the if block because LLIter successfully moved forward
  // or we successfully advanced the iterator to the next bucket.
  return 1;
}

int HTIteratorPastEnd(HTIter iter) {
  Assert333(iter != NULL);

  return !iter->is_valid;
}

int HTIteratorGet(HTIter iter, HTKeyValue *keyvalue) {
  Assert333(iter != NULL);
  // If the HashTable is empty than the iter should be invalid so no need
  // to check hash table size.
  if (!iter->is_valid) {
    return 0;
  }
  HTKeyValue *payload;
  LLIteratorGetPayload(iter->bucket_it, (void **)&payload);
  *keyvalue = *payload;
  return 1; 
}

int HTIteratorDelete(HTIter iter, HTKeyValue *keyvalue) {
  HTKeyValue kv;
  int res, retval;

  Assert333(iter != NULL);

  // Try to get what the iterator is pointing to.
  res = HTIteratorGet(iter, &kv);
  if (res == 0)
    return 0;

  // Advance the iterator.
  res = HTIteratorNext(iter);
  if (res == 0) {
    retval = 2;
  } else {
    retval = 1;
  }
  res = RemoveFromHashTable(iter->ht, kv.key, keyvalue);
  Assert333(res == 1);
  Assert333(kv.key == keyvalue->key);
  Assert333(kv.value == keyvalue->value);

  return retval;
}

int LookupRemoveHashTable(HashTable table, uint64_t key, 
                                HTKeyValue *keyvalue,
                                bool remove) {
  Assert333(table != NULL);
  Assert333(keyvalue != NULL);

  LinkedList chain = GetChainByKey(table, key); 
  if (NumElementsInLinkedList(chain) > 0) {  // Only search list if it's
                                             // nonempty
    LLIter itr = LLMakeIterator(chain, 0);
    if (itr == NULL) {  // LLMakeIterator failed.
      return -1;
    }
    do {  // Iterates through the list searching for the key.
      HTKeyValue *payloadPtr = NULL;
      LLIteratorGetPayload(itr, (void **)&payloadPtr); 
      if (payloadPtr->key == key) {
          *keyvalue = *payloadPtr;  // Set output parameter.
          if (remove) {
            free(payloadPtr);
            LLIteratorDelete(itr, NullFree);  // We return the payload and it 
                                              // is the customers job to free it.
            table->num_elements--;
          }
          LLIteratorFree(itr);
          return 1;
      }
    } while (LLIteratorNext(itr));
    LLIteratorFree(itr);
  }
  // key was not found in table.
  return 0; 
}

static void ResizeHashtable(HashTable ht) {
  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  HashTable newht = AllocateHashTable(ht->num_buckets * 9);

  // Give up if out of memory.
  if (newht == NULL)
    return;

  // Loop through the old ht with an iterator,
  // inserting into the new HT.
  HTIter it = HashTableMakeIterator(ht);
  if (it == NULL) {
    // Give up if out of memory.
    FreeHashTable(newht, &NullFree);
    return;
  }

  while (!HTIteratorPastEnd(it)) {
    HTKeyValue item, dummy;

    Assert333(HTIteratorGet(it, &item) == 1);
    if (InsertHashTable(newht, item, &dummy) != 1) {
      // failure, free up everything, return.
      HTIteratorFree(it);
      FreeHashTable(newht, &NullFree);
      return;
    }
    HTIteratorNext(it);
  }

  // Worked!  Free the iterator.
  HTIteratorFree(it);

  // Sneaky: swap the structures, then free the new table,
  // and we're done.
  {
    HashTableRecord tmp;

    tmp = *ht;
    *ht = *newht;
    *newht = tmp;
    FreeHashTable(newht, &NullFree);
  }

  return;
}
