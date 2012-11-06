/*
 * Tobias Kahan
 * HTTPHeader counter for Extrahop coding interview.
 * Parses a file as a command line line argument and outputs the number of times
 * certain headers are found. The headers are set in main.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "HashTable.h"

/*
 * Increments the header parameter by 1.
 * If header does not exist in the HashTable, adds new kv pair to table
 * and sets initial count to 1.
 * Does no error checking on header or ht.
 */
void IncrementHeaderCount(char *header, HashTable ht);
/*
 * Prints usage statement to stderr.
 */
void Usage(char *executable) {
  fprintf(stderr, "Usage: %s filename\n", executable);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    Usage(argv[0]); 
    return EXIT_FAILURE;
  }
  const char *FILE_HEADER = "<file start>\n";
  const char *FILE_FOOTER = "<file end>\n";
  const char *headers[] = {"Connection", "Accept", "Content-Length"};
  const int numHeaders = 3;
  FILE *f; 
  char *line;
  size_t dummy_size = 0;
  ssize_t bytes_read;
  uint64_t key;
  int retval;
  HTKeyValue oldkv;
  HashTable ht = AllocateHashTable(10);

  f = fopen(argv[1], "rb");
  if (f == NULL) {
    perror("Error opening file for reading");
    return EXIT_FAILURE;
  }
  while ((bytes_read = getline(&line, &dummy_size, f)) != -1) {
    if (strcmp(line, FILE_HEADER) && strcmp(line, FILE_FOOTER)) {
      char *header = strtok(line, ":");
      IncrementHeaderCount(header, ht);
    }
  }
  
  for (int i = 0; i < numHeaders; i++) {
    const char *header = headers[i];
    key = FNVHash64((unsigned char *) header, strlen(header));
    retval = LookupHashTable(ht, key, &oldkv);
    int count = retval ? *((int *)oldkv.value) : 0;
    printf("%s: %d\n", header, count);
  }
  FreeHashTable(ht, &free);
  return EXIT_SUCCESS;
}

void IncrementHeaderCount(char *header, HashTable ht) {
  HTKeyValue kv;
  HTKeyValue oldkv;
  uint64_t key;
  int retval;

  key = FNVHash64((unsigned char *) header, strlen(header));
  retval = LookupHashTable(ht, key, &oldkv);
  kv.key = key;
  int *count = (int *)malloc(sizeof(int));
  *count = 1;
  kv.value = (void *)count;
  InsertHashTable(ht, kv, &oldkv);
  if (retval) {
    int oldval = *((int *)oldkv.value);
    *count += oldval;
    free(oldkv.value);
  }
}
