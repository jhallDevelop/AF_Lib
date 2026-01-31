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

    snprintf(_hashTable->entries[_hashTable->count].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
    _hashTable->entries[_hashTable->count].value.int32Value = _intValue;

    _hashTable->count += 1;
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

    snprintf(_hashTable->entries[_hashTable->count].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
    _hashTable->entries[_hashTable->count].value.uint32Value = _uint;

    _hashTable->count += 1;
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

    snprintf(_hashTable->entries[_hashTable->count].key, AF_HASHTABLE_KEY_MAX_LENGTH, "%s", _key);
    _hashTable->entries[_hashTable->count].value.floatValue = _floatValue;

    _hashTable->count += 1;
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

    uint32_t returnValue = -1;

    for(uint32_t i = 0; i < _hashTable->count; i++){
        if(strncmp(_hashTable->entries[i].key, _key, AF_HASHTABLE_KEY_MAX_LENGTH) == 0){
            returnValue = _hashTable->entries[i].value.int32Value;
            break;
        }
    }
    return returnValue;
}

// ====================
// AF_HashTable_GetUIntValue
// Retrieves the value associated with the given key from the hash table
// ====================
uint32_t AF_HashTable_GetUIntValue(const char* _key, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_GetValue: Hash table is NULL\n");
        return 0;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_GetValue: Key is NULL\n");
        return 0;
    }   

    uint32_t returnValue = 0;
    for(uint32_t i = 0; i < _hashTable->count; i++){
        if(strncmp(_hashTable->entries[i].key, _key, AF_HASHTABLE_KEY_MAX_LENGTH) == 0){
            returnValue = _hashTable->entries[i].value.uint32Value;
        }
    }

    return returnValue;
}

// ====================
// AF_HashTable_GetFloatValue
// Retrieves the value associated with the given key from the hash table
// ====================
AF_FLOAT AF_HashTable_GetFloatValue(const char* _key, AF_HashTable* _hashTable){
    if(_hashTable == NULL){
        AF_Log_Error("AF_HashTable_GetValue: Hash table is NULL\n");
        return 0;
    }

    if(_key == NULL ){
        AF_Log_Error("AF_HashTable_GetValue: Key is NULL\n");
        return 0;
    }   

    AF_FLOAT returnValue = 0.0f;
    for(uint32_t i = 0; i < _hashTable->count; i++){
        if(strncmp(_hashTable->entries[i].key, _key, AF_HASHTABLE_KEY_MAX_LENGTH) == 0){
            returnValue =  _hashTable->entries[i].value.floatValue;
            break;
        }
    }
    return returnValue;
}
