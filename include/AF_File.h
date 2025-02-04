#ifndef AF_FILE_H
#define AF_FILE_H

#include <stdio.h>

#include "AF_Lib_Define.h"
#include "AF_FileList.h"

#ifdef __cplusplus
extern "C" {    
#endif

FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands);
void AF_File_PrintTextBuffer(FILE* _filePtr);
void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize);
void AF_File_CloseFile(FILE* _filePtr);
int AF_File_CompareItemsByValue(const void* lhs, const void* rhs);
void AF_File_OrderAlphabetically(AF_FileList* _fileList);
void AF_File_ListFiles(const char *path, AF_FileList* _fileList, BOOL _isAlphabetical);
#ifdef __cplusplus
}
#endif

#endif // AF_File_H
