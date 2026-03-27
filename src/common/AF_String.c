#include "AF_String.h"
#include <string.h>

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
