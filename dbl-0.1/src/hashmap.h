#ifndef _hashmap_h
#define _hashmap_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 200 // Adjust as needed

// Structure for key-value pairs
typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

// Structure for the hash table
typedef struct HashTable {
    Entry **table; 
} HashTable;

// Function prototypes
HashTable *createHashTable();
void insert(HashTable *ht, char *key, char *value);
char *get(HashTable *ht, char *key);
char **get_all_keys(HashTable *ht, int *num_keys); // New function

#endif