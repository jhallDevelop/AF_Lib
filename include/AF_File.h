#ifndef AF_FILE_H
#define AF_FILE_H
#include "AF_Lib_API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <assert.h>

#include "AF_Lib_Define.h"
#include "AF_FileList.h"
#include "AF_Log.h"
#include "AF_String.h"

#define DEFAULT_CHAR_BUFF_SIZE 1024
#define LARGE_CHAR_BUFF_SIZE 2048

#ifdef __cplusplus
extern "C" {
#endif

// ================================================================
// Platform-specific — implemented in AF_File_posix.c / AF_File_win32.c
// ================================================================

AF_LIB_API FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands);
void AF_File_ListFiles(const char* path, AF_FileList* _fileList, af_bool_t _isAlphabetical);
AF_LIB_API af_bool_t AF_File_MakeDirectory(const char* _filePath);
AF_LIB_API void AF_File_SetWorkingDirectory(const char* projectRoot);

// ================================================================
// Portable — static inline implementations
// ================================================================

static inline void AF_File_PrintTextBuffer(FILE* _filePtr) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
		return;
	}
	char buf[1024];
	size_t nread;
	while ((nread = fread(buf, sizeof(buf[0]), sizeof(buf) - 1, _filePtr)) != 0) {
		buf[nread] = '\0';
		char* s = buf;
		while (s != NULL) {
			char* s_next_line = strchr(s, '\n');
			if (s_next_line != NULL) {
				*s_next_line = '\0';
				AF_Log_Error("%s\n", s);
				s = s_next_line + 1;
			} else {
				AF_Log_Error("%s", s);
				s = NULL;
			}
		}
	}
	if (ferror(_filePtr)) {
		AF_Log_Error("AF_File_PrintTextBuffer: Error while reading\n");
	}
}

static inline void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_WriteFile: FAILED to open file. _filePtr is NULL\n");
		return;
	}
	size_t num_written = fwrite(_data, dataSize, 1, _filePtr);
	if (num_written != 1) {
		AF_Log_Error("AF_File_WriteFile: Error writing to file");
	}
	if (ferror(_filePtr)) {
		AF_Log_Error("AF_File_WriteFile: Error while writing\n");
		return;
	}
}

AF_LIB_API void AF_File_CloseFile(FILE* _filePtr);

static inline int AF_File_CompareItemsByValue(const void* lhs, const void* rhs) {
	const char* a = *(const char**)lhs;
	const char* b = *(const char**)rhs;
	return strcmp(a, b);
}

static inline void AF_File_OrderAlphabetically(AF_FileList* _fileList) {
	if (_fileList->numberOfFiles < 2) {
		_fileList->isSorted = AF_TRUE;
		return;
	}
	char tempBuffer[MAX_FILELIST_BUFFER_SIZE];
	snprintf(tempBuffer, MAX_FILELIST_BUFFER_SIZE, "%s", _fileList->stringBuffer);
	tempBuffer[MAX_FILELIST_BUFFER_SIZE - 1] = '\0';
	char* filePointers[MAX_FILELIST_BUFFER_SIZE] = {0};
	uint32_t fileCount = 0;
	char* savePtr = NULL;
	char* token = AF_StrtokR(tempBuffer, ",", &savePtr);
	while (token != NULL && fileCount < MAX_FILELIST_BUFFER_SIZE) {
		filePointers[fileCount++] = token;
		token = AF_StrtokR(NULL, ",", &savePtr);
	}
	qsort(filePointers, fileCount, sizeof(char*), AF_File_CompareItemsByValue);
	size_t bufferPosition = 0;
	for (uint32_t i = 0; i < fileCount; i++) {
		size_t nameLength = strlen(filePointers[i]);
		snprintf(_fileList->stringBuffer + bufferPosition, MAX_FILELIST_BUFFER_SIZE - bufferPosition, "%s,", filePointers[i]);
		bufferPosition += nameLength + 1;
	}
	if (bufferPosition > 0) {
		_fileList->stringBuffer[bufferPosition - 1] = '\0';
	}
}

AF_LIB_API af_bool_t AF_File_FileExists(const char* _filePath);
AF_LIB_API uint32_t AF_File_GetFileSize(const char* _filePath);
static inline af_bool_t AF_File_ReadFile(char* _buffer, uint32_t _bufferSize, const char* thisFilePath, const char* _readCommand) {
	FILE* _fileOpen = AF_File_OpenFile(thisFilePath, _readCommand);
	if (_fileOpen == NULL) {
		AF_Log_Error("AF_Util: Read File: Failed to read file\n%s\nCheck file exists\n\n", thisFilePath);
		return AF_FALSE;
	}
	if (!_buffer) {
		AF_Log_Error("AF_Util: Read File: Memory allocation failed\n");
		fclose(_fileOpen);
		assert(_bufferSize > 0 && "AF_Util: Read File: Buffer size must be greater than 0\n");
		return AF_FALSE;
	}
	size_t bytesRead = fread(_buffer, 1, _bufferSize, _fileOpen);
	if (ferror(_fileOpen)) {
		AF_Log_Error("AF_Util: Read File: Error reading file\n%s\n", thisFilePath);
		fclose(_fileOpen);
		return AF_FALSE;
	}
	_buffer[bytesRead] = '\0';
	fclose(_fileOpen);
	return AF_TRUE;
}

static inline uint32_t AF_File_GetPathName(const char* _fontPath, char* _buffer, uint32_t _bufferSize) {
	if (_fontPath == NULL || _buffer == NULL || _bufferSize == 0) {
		AF_Log_Error("AF_File_GetPathName: Invalid input parameters\n");
		return 0;
	}
	const char* lastSlash = strrchr(_fontPath, '/');
	const char* lastBackslash = strrchr(_fontPath, '\\');
	const char* lastSeparator = lastSlash > lastBackslash ? lastSlash : lastBackslash;
	size_t pathLength = lastSeparator ? (size_t)(lastSeparator - _fontPath) : strlen(_fontPath);
	if (pathLength >= _bufferSize) {
		AF_Log_Error("AF_File_GetPathName: Buffer too small for path name\n");
		if (_bufferSize > 0) {
			_buffer[0] = '\0';
		}
		return 0;
	}
	AF_STRNCPY_S(_buffer, _bufferSize, _fontPath, pathLength);
	_buffer[pathLength] = '\0';
	return (uint32_t)pathLength;
}

#ifdef __cplusplus
}
#endif

#endif // AF_FILE_H
