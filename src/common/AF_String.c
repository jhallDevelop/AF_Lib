#include "AF_String.h"
#include <string.h>
#include "AF_Log.h"

char* AF_StrtokR(char* src, const char* delimiters, char** savePtr) {
#ifdef _WIN32
    return strtok_s(src, delimiters, savePtr);
#elif defined(__EMSCRIPTEN__) || defined(AF_WEB_BUILD)
    if (src != NULL) {
        *savePtr = src;
    }
    if (*savePtr == NULL) {
        return NULL;
    }
    char* token = strtok(*savePtr, delimiters);
    *savePtr = NULL;
    return token;
#else
    return strtok_r(src, delimiters, savePtr);
#endif
}


// ====================
// AF_String_IsEmpty
// Check if a const char* is empty
// Return AF_TRUE if string is empty
// Return AF_FALSE if string is not empty
// ====================
af_bool_t AF_String_IsEmpty(const char* _string){
    // null check the const char*, then see if the first element is a end of line
    if ((_string == NULL) || (_string[0] == '\0')) {
        return AF_TRUE;
    }else {
        return AF_FALSE;
    }
}

// ====================
// AF_String_Compare
// Compare two strings and 
// return 0 if they are equal
// a positive value if a > b
// a negative value if a < b.
// ====================
int32_t AF_String_Compare(const char* a, const char* b, size_t _maxSize) {
    if (a == NULL || b == NULL) {
        AF_Log_Error("AF_String_Compare: NULL pointer passed to comparison function\n");
        return 0; // Consider them equal to avoid sorting issues
    }

    int32_t result = strncmp(a, b, _maxSize);
    return result;
}
