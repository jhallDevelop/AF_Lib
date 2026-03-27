#ifndef AF_STRING_H
#define AF_STRING_H

#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

// Portable reentrant tokenizer wrapper
char* AF_StrtokR(char* src, const char* delimiters, char** savePtr);

#ifdef __cplusplus
}
#endif

#endif // AF_STRING_H
