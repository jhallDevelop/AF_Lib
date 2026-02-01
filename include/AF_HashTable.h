#ifndef AF_HASHTABLE_H
#define AF_HASHTABLE_H
#include <stdalign.h>
#include "AF_Lib_Define.h"

#define AF_HASHTABLE_KEY_MAX_LENGTH 64
#define AF_HASHTABLE_MAX_ENTRIES 512
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

union AF_HASHTABLE_VALUE {
    uint32_t uint32Value;
    int32_t int32Value;
    AF_FLOAT floatValue;
};
// Inspired by james routley https://github.com/jamesroutley/write-a-hash-table/tree/master/02-hash-table

typedef struct AF_HashTableEntry { 
    char key[AF_HASHTABLE_KEY_MAX_LENGTH];         // key string with max AF_HASHTABLE_KEY_MAX_LENGTH
    union AF_HASHTABLE_VALUE value;                // Union to hold different value types
} AF_HashTableEntry;

typedef struct AF_HashTable {
    uint32_t size;                                          // size of the hash table (number of buckets)
    uint32_t count;                                         // number of entries in the hash table
    AF_HashTableEntry entries[AF_HASHTABLE_MAX_ENTRIES];    // Fixed array of hash table entries
} AF_HashTable;

AF_HashTableEntry AF_HashTableEntry_ZERO(void);
AF_HashTable AF_HashTable_ZERO(void);
AF_HashTableEntry* AF_HashTable_NewIntEntry(const char* _key, int32_t _value, AF_HashTable* _hashTable);
AF_HashTableEntry* AF_HashTable_NewUIntEntry(const char* _key, uint32_t _value, AF_HashTable* _hashTable);
AF_HashTableEntry* AF_HashTable_NewFloatEntry(const char* _key, AF_FLOAT _value, AF_HashTable* _hashTable);

int32_t AF_HashTable_GetIntValue(const char* _key, AF_HashTable* _hashTable);
int32_t AF_HashTable_GetUIntValue(const char* _key, AF_HashTable* _hashTable);
AF_FLOAT AF_HashTable_GetFloatValue(const char* _key, AF_HashTable* _hashTable);
uint64_t AF_HashTable_HashKey(const char* _key);


#endif  // AF_HASHTABLE_H
