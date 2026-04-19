#ifndef AF_FILE_H
#define AF_FILE_H
#include "AF_Lib_API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>

#include "AF_Lib_Define.h"
#include "AF_FileList.h"
#include "AF_Log.h"
#include "AF_String.h"

// TODO: refactor this file to be more modular and split into multiple files if needed, as it grows in complexity. 
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define AF_PROJECT_GETCWD _getcwd
#define AF_PROJECT_ACCESS _access
#else
#include <unistd.h>
#define AF_PROJECT_GETCWD getcwd
#define AF_PROJECT_ACCESS access
#endif

#define DEFAULT_CHAR_BUFF_SIZE 1024
#define LARGE_CHAR_BUFF_SIZE 2048

#ifdef __cplusplus
extern "C" {
#endif

// ================================================================
// Platform-specific — implemented in AF_File_posix.c / AF_File_win32.c
// ================================================================

AF_LIB_API FILE* AF_File_Open(const char* path, const char* mode);
AF_LIB_API af_bool_t AF_File_Close(FILE* _filePtr);
AF_LIB_API af_bool_t AF_File_Read(char* _buffer, uint32_t _bufferSize, const char* thisFilePath, const char* _readCommand);
AF_LIB_API void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize);
AF_LIB_API void AF_File_ListFiles(const char* path, AF_FileList* _fileList, af_bool_t _isAlphabetical);
AF_LIB_API af_bool_t AF_File_MakeDirectory(const char* _filePath);
AF_LIB_API af_bool_t AF_File_ChangeDirectory(const char* path);
int32_t AF_File_CompareItemsByValue(const void* lhs, const void* rhs);
AF_LIB_API void AF_File_NormalisePath(char* path);
AF_LIB_API af_bool_t AF_File_RelativizePath(const char* _inPath, const char* _projectRoot, char* _outPath, uint32_t _outPathSize);
AF_LIB_API af_bool_t AF_File_IsAbsolutePath(const char* _path);
AF_LIB_API af_bool_t AF_File_PathExists(const char* _path);
AF_LIB_API void AF_File_ConvertPathSeparators(char* outPath, uint32_t outSize, const char* inPath, char from, char to);
AF_LIB_API af_bool_t AF_File_HasPrefixIgnoreCase(const char* str, const char* prefix);
AF_LIB_API af_bool_t AF_File_ResolveCrossPlatformAbsolutePath(const char* sourcePath, char* outPath, uint32_t outSize);
AF_LIB_API void AF_File_MakeAbsolutePath(char* _path, uint32_t _pathSize, const char* _baseDir) ;
AF_LIB_API void AF_File_GetFileStemNoExt(const char* _fileName, char* _outStem, uint32_t _outStemSize);
AF_LIB_API void AF_File_GetDirectoryPath(const char* _filePath, char* _directoryPath, uint32_t _directoryPathSize);
AF_LIB_API void AF_File_PrintTextBuffer(FILE* _filePtr);
AF_LIB_API void AF_File_OrderAlphabetically(AF_FileList* _fileList);
AF_LIB_API af_bool_t AF_File_Exists(const char* _filePath);
AF_LIB_API uint32_t AF_File_GetFileSize(const char* _filePath);
AF_LIB_API uint32_t AF_File_GetPathName(const char* _fontPath, char* _buffer, uint32_t _bufferSize);
AF_LIB_API void AF_File_GetFileNameOnly(const char* _path, char* _outFileName, uint32_t _outFileNameSize);



#ifdef __cplusplus
}
#endif

#endif // AF_FILE_H
