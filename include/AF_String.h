#ifndef AF_STRING_H
#define AF_STRING_H

#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

// Portable reentrant tokenizer wrapper
char* AF_StrtokR(char* src, const char* delimiters, char** savePtr);
af_bool_t AF_String_IsEmpty(const char* _string);
int32_t AF_String_Compare(const char* a, const char* b, size_t _maxSize);

#ifdef __cplusplus
}
#endif

#endif // AF_STRING_H
