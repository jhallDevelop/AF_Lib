#include "AF_HashTable.h"
#include "AF_Util.h"
// ====================
// AF_HashTableEntry_ZERO
// Returns a zeroed hash table entry
// ====================
AF_HashTableEntry AF_HashTableEntry_ZERO(void){
    AF_HashTableEntry returnEntry;
    snprintf(returnEntry.key, AF_HASHTABLE_KEY_MAX_LENGTH, "");
    returnEntry.value.uint32Value = 0;
    return returnEntry;
}

// ====================
// AF_HashTable_ZERO
// Returns a zeroed hash table
// ====================
AF_HashTable AF_HashTable_ZERO(void){
    AF_HashTable returnHashTable;
    returnHashTable.size = 0;
    returnHashTable.count = 0;
    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++){
        returnHashTable.entries[i] = AF_HashTableEntry_ZERO();
    }
    
    return returnHashTable;
}



// ====================
// AF_HashTable_NewIntEntry
// Creates a new int entry in the hash table with the given key and value
// ====================
AF_HashTableEntry* AF_HashTable_NewIntEntry(const char* _key, int32_t _intValue, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_NewEntry: Hash table is NULL\n");
        return NULL;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_NewEntry: Key or value is NULL\n");
        return NULL;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;
    
    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++){
        uint32_t tryIndex = (hashModulo + i) % AF_HASHTABLE_MAX_ENTRIES;

        // 1: is it empty or empty key? (no collision, and save to break the loop after adding
        if((_hashTable->entries[tryIndex].key[0] == '\0') || (strcmp(_hashTable->entries[tryIndex].key, _key) == 0)){
            if(_hashTable->entries[tryIndex].key[0] == '\0'){
                _hashTable->count += 1;
            }

            snprintf(_hashTable->entries[tryIndex].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
            _hashTable->entries[tryIndex].value.int32Value = _intValue;
            return &_hashTable->entries[tryIndex];
        }
    }
    AF_Log_Error("AF_HashTable_NewIntEntry: Unable to resolve collision for key '%s'\n", _key);
    return NULL;
}

// ====================
// AF_HashTable_NewUIntEntry
// Creates a new uint entry in the hash table with the given key and value
// ====================
AF_HashTableEntry* AF_HashTable_NewUIntEntry(const char* _key, uint32_t _uint, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_NewEntry: Hash table is NULL\n");
        return NULL;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_NewEntry: Key or value is NULL\n");
        return NULL;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;
    
    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++){
        uint32_t tryIndex = (hashModulo + i) % AF_HASHTABLE_MAX_ENTRIES;

        // 1: is it empty or empty key? (no collision, and save to break the loop after adding
        if((_hashTable->entries[tryIndex].key[0] == '\0') || (strcmp(_hashTable->entries[tryIndex].key, _key) == 0)){
            if(_hashTable->entries[tryIndex].key[0] == '\0'){
                _hashTable->count += 1;
            }

            snprintf(_hashTable->entries[tryIndex].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
            _hashTable->entries[tryIndex].value.uint32Value = _uint;
            return &_hashTable->entries[tryIndex];
        }
    }
    AF_Log_Error("AF_HashTable_NewIntEntry: Unable to resolve collision for key '%s'\n", _key);
    return NULL;
}

// ====================
// AF_HashTable_NewFloatEntry
// Creates a new float entry in the hash table with the given key and value
// ====================
AF_HashTableEntry* AF_HashTable_NewFloatEntry(const char* _key, AF_FLOAT _floatValue, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_NewEntry: Hash table is NULL\n");
        return NULL;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_NewEntry: Key or value is NULL\n");
        return NULL;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;
    
    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++){
        uint32_t tryIndex = (hashModulo + i) % AF_HASHTABLE_MAX_ENTRIES;

        // 1: is it empty or empty key? (no collision, and save to break the loop after adding
        if((_hashTable->entries[tryIndex].key[0] == '\0') || (strcmp(_hashTable->entries[tryIndex].key, _key) == 0)){
            if(_hashTable->entries[tryIndex].key[0] == '\0'){
                _hashTable->count += 1;
            }

            snprintf(_hashTable->entries[tryIndex].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
            _hashTable->entries[tryIndex].value.floatValue = _floatValue;
            return &_hashTable->entries[tryIndex];
        }
    }
    AF_Log_Error("AF_HashTable_NewIntEntry: Unable to resolve collision for key '%s'\n", _key);
    return NULL;
}


// ====================
// AF_HashTable_GetIntValue
// Retrieves the value associated with the given key from the hash table
// ====================
int32_t AF_HashTable_GetIntValue(const char* _key, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_GetValue: Hash table is NULL\n");
        return -1;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_GetValue: Key is NULL\n");
        return -1;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;


    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++) {
        uint32_t tryIndex = (hashModulo + i) % AF_HASHTABLE_MAX_ENTRIES;
        
        // If we hit an empty slot, the key definitely isn't in the table
        if(_hashTable->entries[tryIndex].key[0] == '\0') return -1;

        if(strcmp(_hashTable->entries[tryIndex].key, _key) == 0) {
            return _hashTable->entries[tryIndex].value.int32Value; // Corrected to int32Value
        }
    }
    return -1;
}

// ====================
// AF_HashTable_GetUIntValue
// Retrieves the value associated with the given key from the hash table
// ====================
int32_t AF_HashTable_GetUIntValue(const char* _key, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_GetValue: Hash table is NULL\n");
        return -1;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_GetValue: Key is NULL\n");
        return -1;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;

    for(uint32_t i = 0; i < AF_HASHTABLE_MAX_ENTRIES; i++) {
        uint32_t tryIndex = (hashModulo + i) % AF_HASHTABLE_MAX_ENTRIES;
        
        if(_hashTable->entries[tryIndex].key[0] == '\0') return 0.0f;

        if(strcmp(_hashTable->entries[tryIndex].key, _key) == 0) {
            return _hashTable->entries[tryIndex].value.floatValue;
        }
    }
    return -1;
}

// ====================
// AF_HashTable_GetFloatValue
// Retrieves the value associated with the given key from the hash table
// ====================
AF_FLOAT AF_HashTable_GetFloatValue(const char* _key, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_GetValue: Hash table is NULL\n");
        return -1;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_GetValue: Key is NULL\n");
        return -1;
    }   

    uint64_t hashKey = AF_HashTable_HashKey(_key);
    uint32_t hashModulo = hashKey % AF_HASHTABLE_MAX_ENTRIES;
    if(hashModulo >= AF_HASHTABLE_MAX_ENTRIES){
        AF_Log_Error("AF_HashTable_GetIntValue: Hash index out of bounds\n");
        return -1;
    }

    // Verify that the key at the hashed index matches the requested key
    if (strcmp(_hashTable->entries[hashModulo].key, _key) == 0) {
        return _hashTable->entries[hashModulo].value.floatValue;
    }
    AF_FLOAT returnValue = -1;

    return returnValue;
}

// ====================
// AF_HashTable_HashKey
// Hashes a key using FNV-1a algorithm
// ====================
uint64_t AF_HashTable_HashKey(const char* _key){
    if(_key == NULL){
        AF_Log_Error("AF_HashTable_HashKey: Key is NULL\n");
        return 0;
    }
    uint64_t hash = FNV_OFFSET;
    for(const char* p = _key; *p; p++){
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

