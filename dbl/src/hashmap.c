// hashmap.c

#include "hashmap.h"

// Simple hash function 
unsigned int hash(char *key) {
    unsigned int hashVal = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hashVal = hashVal * 37 + key[i]; 
    }
    return hashVal % TABLE_SIZE;
}

// Create a new hash table
HashTable *createHashTable() {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (ht == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    ht->table = (Entry **)malloc(sizeof(Entry *) * TABLE_SIZE);
    if (ht->table == NULL) {
        perror("Memory allocation failed");
        free(ht); // Free the previously allocated HashTable
        return NULL;
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

// Insert a key-value pair
void insert(HashTable *ht, char *key, char *value) {
    unsigned int index = hash(key);
    Entry *newEntry = (Entry *)malloc(sizeof(Entry));
    if (newEntry == NULL) {
        perror("Memory allocation failed");
        return; 
    }
    newEntry->key = strdup(key); 
    if (newEntry->key == NULL) {
        perror("Memory allocation failed");
        free(newEntry);
        return; 
    }
    newEntry->value = strdup(value);
    if (newEntry->value == NULL) {
        perror("Memory allocation failed");
        free(newEntry->key);
        free(newEntry);
        return; 
    }
    newEntry->next = ht->table[index];
    ht->table[index] = newEntry;
}

// Retrieve a value by key
char *get(HashTable *ht, char *key) {
    unsigned int index = hash(key);
    Entry *entry = ht->table[index];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL; // Not found
}

// Get all keys from the hash table
char **get_all_keys(HashTable *ht, int *num_keys) {
    int count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = ht->table[i];
        while (entry != NULL) {
            count++;
            entry = entry->next;
        }
    }

    char **keys = (char **)malloc(sizeof(char *) * count);
    if (keys == NULL) {
        perror("Memory allocation failed");
        *num_keys = 0;
        return NULL;
    }

    int index = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = ht->table[i];
        while (entry != NULL) {
            keys[index++] = strdup(entry->key); 
            entry = entry->next;
        }
    }

    *num_keys = count;
    return keys;
}