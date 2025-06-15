#ifndef AF_FILE_H
#define AF_FILE_H
#include "AF_Lib_API.h"
#include <stdio.h>

#include "AF_Lib_Define.h"
#include "AF_FileList.h"

#define DEFAULT_CHAR_BUFF_SIZE 1024
#define LARGE_CHAR_BUFF_SIZE 2048

#ifdef __cplusplus
extern "C" {    
#endif

AF_LIB_API FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands);
void AF_File_PrintTextBuffer(FILE* _filePtr);
void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize);
void AF_File_CloseFile(FILE* _filePtr);
int AF_File_CompareItemsByValue(const void* lhs, const void* rhs);
void AF_File_OrderAlphabetically(AF_FileList* _fileList);
void AF_File_ListFiles(const char *path, AF_FileList* _fileList, af_bool_t _isAlphabetical);
af_bool_t AF_File_FileExists(const char* _filePath);
af_bool_t AF_File_MakeDirectory(const char* _filePath);
af_bool_t AF_File_ReadFile(char* _buffer, uint32_t _bufferSize, const char* thisFilePath, const char* _readCommand);
uint32_t AF_File_GetFileSize(const char* _filePath);

#ifdef __cplusplus
}
#endif

#endif // AF_File_H
