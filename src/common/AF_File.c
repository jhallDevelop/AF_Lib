#include "AF_File.h"

#include "AF_Log.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AF_String.h"



// ================================================================
// Private helpers
// ================================================================


// =================================================
// AF_File_Open
// safely open a file with error handling and logging
// =================================================
FILE* AF_File_Open(const char* path, const char* mode) {
    if (AF_String_IsEmpty(path) == AF_TRUE) {
        AF_Log_Error("AF_File_Open: Empty file path provided\n");
        return NULL;
    }

    if(mode == NULL || mode[0] == '\0') {
        AF_Log_Error("AF_File_Open: Empty file mode provided\n");
        return NULL;
    }

    if(AF_File_PathExists(path) == AF_FALSE) {
        AF_Log_Error("AF_File_Open: File does not exist: %s\n", path);
        return NULL;
    }

    FILE* filePtr = NULL;
    int32_t result = 0;
    #if defined(_WIN64)
        result = fopen_s(&filePtr, path, mode);
    #else
        filePtr = fopen(path, mode);
    #endif
    if (result != 0 || filePtr == NULL) {  
        AF_Log_Error("AF_File_Open: Failed to open file: %s with mode: %s\n", path, mode);
        return NULL;
    }
    

	return filePtr;
}


// =================================================
// AF_File_Close
// Closes a file pointer, checking for errors.
// =================================================
af_bool_t AF_File_Close(FILE* _filePtr) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_CloseFile: FAILED to close buffer. _filePtr is NULL\n");
		return AF_FALSE;
	}

	if (ferror(_filePtr)) {
		AF_Log_Warning("AF_File_CloseFile: File had errors before closing\n");
        return AF_FALSE;
	}

	if (fclose(_filePtr) != 0) {
		AF_Log_Error("AF_File_CloseFile: Error while closing file\n");
        return AF_FALSE;
	}
	return AF_TRUE;
}



// ================================
// AF_File_ChangeDirectory
// Changes the current working directory to the specified path.
// Returns AF_TRUE on success, AF_FALSE on failure.
// ================================
af_bool_t AF_File_ChangeDirectory(const char* path) {
	if (!path || path[0] == '\0') {
        return AF_FALSE;
    }

    int32_t result = 0;
    // Platform dependent
    #ifdef _WIN32
        result = _chdir(path) == -1;
    #else
        result = chdir(path) == -1;
    #endif

	if (result == 0) {
		AF_Log("AF_File_ChangeDirectory: SUCCESS: %s\n", path);
		return AF_TRUE;
	}
	return AF_FALSE;
}

// ================================
// AF_File_NormalizePathSeparators
// Converts all occurrences of 'from' character to 'to' character in the input path.
// This is used to ensure consistent path formats across platforms and codebase.
// ================================
void AF_File_NormalizePathSeparators(char* outPath, size_t outSize, const char* inPath, char from, char to) {
	if (!outPath || !inPath || outSize == 0) return;
	size_t i = 0;
	for (; i + 1 < outSize && inPath[i] != '\0'; ++i) {
		char c = inPath[i];
		outPath[i] = (c == from ? to : c);
	}
	outPath[i] = '\0';
}


af_bool_t AF_File_PathHasPrefix(const char* path, const char* prefix) {
	if (!path || !prefix) return AF_FALSE;
	size_t prefixLen = strlen(prefix);
	return (strlen(path) >= prefixLen && strncmp(path, prefix, prefixLen) == 0) ? AF_TRUE : AF_FALSE;
}


// ================================================================
// AF_File_NormalisePath
// Should always be called before any file operations to ensure consistent path formats across platforms and codebase.
// ================================================================
void AF_File_NormalisePath(char* path) {
    if (!path) 
    {
        return;
    }

    // ---- WINDOWS ----
    // Convert all '/' to '\\' on Windows, as Windows APIs typically expect backslashes as path separators.
    #ifdef _WIN32
        for (char* p = path; *p; ++p) {
            if (*p == '/') {
                *p = '\\';
            }
        }
        return;
    #endif

    // No-op on posix platforms, as they typically use '/' as the path separator.
    (void)path; // No-op to avoid unused function warning if not used on this platform
}




// ================================
// AF_File_ListFiles
// Enumerate directory entries with opendir/readdir.
// Windows implementation uses FindFirstFileA/FindNextFileA for better performance and to avoid issues with long paths and Unicode.
// Posix implementation uses opendir/readdir for simplicity and compatibility.
// =================================
void AF_File_ListFiles(const char* path, AF_FileList* _fileList, af_bool_t _isAlphabetical) {
	if (MAX_FILELIST_BUFFER_SIZE > 0) _fileList->stringBuffer[0] = '\0';
	_fileList->numberOfFiles = 0;

    #ifdef _WIN32
        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        char searchPath[AF_MAX_PATH_CHAR_SIZE];
        snprintf(searchPath, sizeof(searchPath), "%s\\*", path);

        hFind = FindFirstFileA(searchPath, &findFileData);
        if (hFind == INVALID_HANDLE_VALUE) return;

        size_t bufferPosition = 0;
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                size_t nameLength = strlen(findFileData.cFileName);
                if (bufferPosition + nameLength + 2 > MAX_FILELIST_BUFFER_SIZE) {
                    AF_Log_Error("AF_File_ListFiles: Buffer overflow. Stopping file accumulation.\n");
                    break;
                }
                snprintf(_fileList->stringBuffer + bufferPosition, MAX_FILELIST_BUFFER_SIZE - bufferPosition, "%s,", findFileData.cFileName);
                bufferPosition += nameLength + 1;
                _fileList->numberOfFiles++;
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);

        FindClose(hFind);

    #else
        DIR* dir = opendir(path);
        if (!dir) return;

        struct dirent* dp;
        size_t bufferPosition = 0;
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;
            size_t nameLength = strlen(dp->d_name);
            if (bufferPosition + nameLength + 2 > MAX_FILELIST_BUFFER_SIZE) {
                AF_Log_Error("AF_File_ListFiles: Buffer overflow. Stopping file accumulation.\n");
                break;
            }
            snprintf(_fileList->stringBuffer + bufferPosition, MAX_FILELIST_BUFFER_SIZE - bufferPosition, "%s,", dp->d_name);
            bufferPosition += nameLength + 1;
            _fileList->numberOfFiles++;
        }
        closedir(dir);
    #endif

	if (_fileList->numberOfFiles > 0) _fileList->stringBuffer[bufferPosition - 1] = '\0';
	if (_isAlphabetical == AF_TRUE) AF_File_OrderAlphabetically(_fileList);
}



// ================================
// AF_File_MakeDirectory
// Create a directory with full permissions.
// ================================
af_bool_t AF_File_MakeDirectory(const char* _filePath) {
    int32_t result = 0;
	#ifdef _WIN32
        result = _mkdir(_filePath) == -1;
    #else
        result = mkdir(_filePath, 0777) == -1;
    #endif
    if (result != 0) {
        AF_Log_Error("AF_Util_MakeFolder: Failed to create directory %s\n", _filePath);
        return AF_FALSE;
    }
    return AF_TRUE;
}


// ================================================
// AF_File_Exists
// Checks if a file exists at the given path.
// ================================================
af_bool_t AF_File_Exists(const char* _filePath) {
	if (_filePath == NULL || _filePath[0] == '\0') {
		return AF_FALSE;
	}

    // Use stat to check for file existence, which works for both files and directories and handles long paths on Windows.
	struct stat fileStat;
	if (stat(_filePath, &fileStat) == 0) {
		return AF_TRUE;
	}
	return AF_FALSE;
}


// ================================================
// AF_File_GetFileSize
// Returns the size of the file at the given path, or 0 if the file does not exist or an error occurs.
// ================================================
uint32_t AF_File_GetFileSize(const char* _filePath) {
	if (_filePath == NULL || _filePath[0] == '\0') {
        AF_Log_Error("AF_Util: empty path: Failed to get file size for %s\n", _filePath);
		return 0;
	}

    // Use stat to get the file size, which works for both files and directories and handles long paths on Windows.
	struct stat fileStat;
	if (stat(_filePath, &fileStat) != 0) {
        AF_Log_Error("AF_Util: Failed to get file size for %s\n", _filePath);
		return 0;
	}

    // Check if it's a regular file
	if (fileStat.st_size < 0) {
        AF_Log_Error("AF_Util: Size of file: Failed to get file size for %s\n", _filePath);
		return 0;
	}

	return (uint32_t)fileStat.st_size;
}



// ================================
// AF_File_PrintTextBuffer
// Prints the contents of a text file to the log, line by line.
// ================================ 
void AF_File_PrintTextBuffer(FILE* _filePtr) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
		return;
	}
	char buf[1024];
	size_t nread;

    // Read the file line by line and log each line. This handles large files without loading the entire content into memory.
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


// ================================
// AF_File_WriteFile
// Writes data to a file, checking for errors.
// ================================
void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_WriteFile: FAILED to open file. _filePtr is NULL\n");
		return;
	}

    // Write the data to the file and check for errors.
	size_t num_written = fwrite(_data, dataSize, 1, _filePtr);
	if (num_written != 1) {
		AF_Log_Error("AF_File_WriteFile: Error writing to file");
	}
	if (ferror(_filePtr)) {
		AF_Log_Error("AF_File_WriteFile: Error while writing\n");
		return;
	}
}


// ================================
// AF_File_CompareItemsByValue
// Comparison function for qsort to sort file names alphabetically.
// ================================ 
int32_t AF_File_CompareItemsByValue(const void* lhs, const void* rhs) {

    if(lhs == NULL || rhs == NULL) {
        AF_Log_Error("AF_File_CompareItemsByValue: NULL pointer passed to comparison function\n");
        return 0; // Consider them equal to avoid sorting issues
    }
    // Cast the void pointers to char** to compare the strings they point to.
	const char* a = *(const char**)lhs;
	const char* b = *(const char**)rhs;
    // Use the AF_String_Compare function to compare the two strings, with a maximum size of AF_MAX_PATH_CHAR_SIZE to prevent buffer overflows.
    int32_t result = AF_String_Compare(a, b, AF_MAX_PATH_CHAR_SIZE); 
	return result;
}

// ================================
// AF_File_OrderAlphabetically
// Sorts the file list alphabetically using qsort.
// ================================
void AF_File_OrderAlphabetically(AF_FileList* _fileList) {
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

    // Tokenize the file list string and store pointers to each file name in an array for sorting.
	while (token != NULL && fileCount < MAX_FILELIST_BUFFER_SIZE) {
		filePointers[fileCount++] = token;
		token = AF_StrtokR(NULL, ",", &savePtr);
	}

    // Sort the file name pointers alphabetically using qsort and the comparison function.
	qsort(filePointers, fileCount, sizeof(char*), AF_File_CompareItemsByValue);
	size_t bufferPosition = 0;
	for (uint32_t i = 0; i < fileCount; i++) {
		size_t nameLength = strlen(filePointers[i]);
		snprintf(_fileList->stringBuffer + bufferPosition, MAX_FILELIST_BUFFER_SIZE - bufferPosition, "%s,", filePointers[i]);
		bufferPosition += nameLength + 1;
	}

    // Ensure the string buffer is null-terminated and does not end with an extra comma.
	if (bufferPosition > 0) {
		_fileList->stringBuffer[bufferPosition - 1] = '\0';
	}
}


// ================================
// AF_File_Read
// Reads the contents of a file into a buffer, checking for errors.
// ================================
af_bool_t AF_File_Read(char* _buffer, uint32_t _bufferSize, const char* thisFilePath, const char* _readCommand) {
	FILE* _fileOpen = AF_File_Open(thisFilePath, _readCommand);
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

// ================================
// AF_File_GetPathName
// Extracts the directory path from a full file path and stores it in the provided buffer.
// Returns the length of the directory path, or 0 if an error occurs.
// ================================
uint32_t AF_File_GetPathName(const char* _fontPath, char* _buffer, uint32_t _bufferSize) {
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

// ===============
// AF_File_IsAbsolutePath
// Checks if a given path is an absolute path based on platform-specific rules.
// ===============
af_bool_t AF_File_IsAbsolutePath(const char* _path) {
    if (_path == NULL || _path[0] == '\0') {
        return AF_FALSE;
    }

    // On Unix-like systems, absolute paths start with '/'
    if (_path[0] == '/' || _path[0] == '\\') {
        return AF_TRUE;
    }

    // On Windows, also check for drive letter paths like "C:\"
    #ifdef _WIN32
    // Check for drive letter followed by colon (e.g., "C:\")
        if (strlen(_path) > 1 && _path[1] == ':') {
            return AF_TRUE;
        }
    #endif

    return AF_FALSE;
}

// ===============
// AF_File_PathExists
// Checks if a given path exists in the filesystem.
// Returns AF_TRUE if the path exists, AF_FALSE otherwise.
// This function uses platform-specific APIs to check for the existence of the path.
// On Windows, it uses _access, and on POSIX systems, it uses access.
// ===============
af_bool_t AF_File_PathExists(const char* _path) {
    if (_path == NULL || _path[0] == '\0') {
        return AF_FALSE;
    }
    return (AF_Project_ACCESS(_path, 0) == 0) ? AF_TRUE : AF_FALSE;
}

// ================
// AF_File_ConvertPathSeparators
// Convert path separators in a string.
// This function takes an input string and replaces all occurrences of a specified character (from) with another character (to).
// The result is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ================
void AF_File_ConvertPathSeparators(char* outPath, uint32_t outSize, const char* inPath, char from, char to) {
    if (outPath == NULL || inPath == NULL || outSize == 0) {
        return;
    }
    
    for (size_t i = 0; i + 1 < outSize && inPath[i] != '\0'; ++i) {
        char c = inPath[i];
        outPath[i] = (c == from ? to : c);
    }

    // Ensure null termination
    if(strlen(inPath) < outSize) {
        outPath[strlen(inPath)] = '\0';
    } else {
        outPath[outSize - 1] = '\0';
    }
}

// ================
// AF_File_RelativizePath
// Requests a scene change by setting the pendingScenePath in the app data.
// This function is used to signal that a scene change should occur, and the actual change will be handled in the main update loop of the application.
// The function takes the app data and the path to the new scene as parameters, and it updates the app data accordingly.
// ===============
af_bool_t AF_File_RelativizePath(const char* _inPath, const char* _projectRoot, char* _outPath, uint32_t _outPathSize) {
    if (_inPath == NULL || _outPath == NULL || _outPathSize == 0) {
        return AF_FALSE;
    }

    if (_inPath[0] == '\0') {
        _outPath[0] = '\0';
        return AF_FALSE;
    }

    // Preserve already-relative paths (non-absolute) as-is.
    if (!AF_File_IsAbsolutePath(_inPath)) {
        // Normalize separators to '/' for storage
        char normalized[AF_MAX_PATH_CHAR_SIZE] = {0};
        AF_File_ConvertPathSeparators(normalized, sizeof(normalized), _inPath, '\\', '/');
        snprintf(_outPath, _outPathSize, "%s", normalized);
        return AF_TRUE;
    }

    if (_projectRoot == NULL || _projectRoot[0] == '\0') {
        // No project root provided, keep absolute as-is
        snprintf(_outPath, _outPathSize, "%s", _inPath);
        return AF_FALSE;
    }

    // Normalize path separators for comparison
    char normalizedInput[AF_MAX_PATH_CHAR_SIZE] = {0};
    AF_File_ConvertPathSeparators(normalizedInput, sizeof(normalizedInput), _inPath, '\\', '/');
    char normalizedRoot[AF_MAX_PATH_CHAR_SIZE] = {0};
    AF_File_ConvertPathSeparators(normalizedRoot, sizeof(normalizedRoot), _projectRoot, '\\', '/');

    // Ensure root ends with '/' for exact matching
    size_t rootLen = strlen(normalizedRoot);
    char rootWithSlash[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (rootLen > 0 && normalizedRoot[rootLen - 1] != '/') {
        snprintf(rootWithSlash, sizeof(rootWithSlash), "%s/", normalizedRoot);
    } else {
        snprintf(rootWithSlash, sizeof(rootWithSlash), "%s", normalizedRoot);
    }

    if (strncmp(normalizedInput, rootWithSlash, strlen(rootWithSlash)) == 0) {
        const char* relStart = normalizedInput + strlen(rootWithSlash);
        if (*relStart == '\0') {
            snprintf(_outPath, _outPathSize, "%s", "");
            return AF_TRUE;
        }
        snprintf(_outPath, _outPathSize, "%s", relStart);
        return AF_TRUE;
    }

    // If not under project root, keep normalized absolute path
    snprintf(_outPath, _outPathSize, "%s", normalizedInput);
    return AF_FALSE;
}

// ================
// AF_File_HasPrefixIgnoreCase
// Resolves the project root path to an absolute path based on the editor app data.
// This function checks if the project root is already an absolute path. If it is, it simply copies it to the output buffer.
// If it is not an absolute path, it combines the editor's root directory with the project root to form an absolute path.
// The resolved absolute path is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ================
af_bool_t AF_File_HasPrefixIgnoreCase(const char* str, const char* prefix) {
    if (!str || !prefix) {
        return AF_FALSE;
    }
    while (*prefix && *str) {
        char a = (char)tolower((unsigned char)*str);
        char b = (char)tolower((unsigned char)*prefix);
        if (a != b) {
            return AF_FALSE;
        }
        str++;
        prefix++;
    }
    return (*prefix == '\0') ? AF_TRUE : AF_FALSE;
}

// ===============
// AF_File_ResolveCrossPlatformAbsolutePath
// Resolves the project root path to an absolute path based on the editor app data.
// This function checks if the project root is already an absolute path. If it is, it simply copies it to the output buffer.
// If it is not an absolute path, it combines the editor's root directory with the project root to form an absolute path.
// The resolved absolute path is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ===============
af_bool_t AF_File_ResolveCrossPlatformAbsolutePath(const char* sourcePath, char* outPath, uint32_t outSize) {
    if (!sourcePath || !outPath || outSize == 0) {
        return AF_FALSE;
    }

    // 1) Exact path exists
    if (AF_File_PathExists(sourcePath)) {
        snprintf(outPath, outSize, "%s", sourcePath);
        return AF_TRUE;
    }

    // 2) Normalize separator to current platform and test
    char nativePath[AF_MAX_PATH_CHAR_SIZE] = {0};
#ifdef _WIN32
    AF_File_ConvertPathSeparators(nativePath, sizeof(nativePath), sourcePath, '/', '\\');
#else
    AF_File_ConvertPathSeparators(nativePath, sizeof(nativePath), sourcePath, '\\', '/');
#endif

    if (AF_File_PathExists(nativePath)) {
        snprintf(outPath, outSize, "%s", nativePath);
        return AF_TRUE;
    }


    // no translation found
    return AF_FALSE;
}

// ================
// AF_File_MakeAbsolutePath
// Converts a given path to an absolute path based on a provided base directory.
// If the input path is already absolute, it resolves it to a canonical absolute path.
// If the input path is relative, it combines it with the base directory to form an absolute path.
// The resulting absolute path is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ================
void AF_File_MakeAbsolutePath(char* _path, uint32_t _pathSize, const char* _baseDir) {
    if (_path == NULL || _path[0] == '\0' || _baseDir == NULL || _baseDir[0] == '\0') {
        return;
    }

    if (AF_File_IsAbsolutePath(_path) == AF_TRUE) {
        char resolved[AF_MAX_PATH_CHAR_SIZE] = {0};
        if (AF_File_ResolveCrossPlatformAbsolutePath(_path, resolved, sizeof(resolved))) {
            snprintf(_path, _pathSize, "%s", resolved);
        }
        return;
    }

    char originalPath[AF_MAX_PATH_CHAR_SIZE];
    snprintf(originalPath, sizeof(originalPath), "%s", _path);
    snprintf(_path, _pathSize, "%s/%s", _baseDir, originalPath);
}

// ================
// AF_File_GetDirectoryPath
// Extracts the directory path from a full file path and stores it in the provided buffer.
// This function takes a full file path as input and extracts the directory portion of the path, excluding the file name.
// The extracted directory path is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ================
void AF_File_GetDirectoryPath(const char* _filePath, char* _directoryPath, uint32_t _directoryPathSize) {
    if (_directoryPath == NULL || _directoryPathSize == 0) {
        return;
    }

    _directoryPath[0] = '\0';
    if (_filePath == NULL || _filePath[0] == '\0') {
        return;
    }

    snprintf(_directoryPath, _directoryPathSize, "%s", _filePath);
    char* lastSlash = strrchr(_directoryPath, '/');
    char* lastBackslash = strrchr(_directoryPath, '\\');
    char* separator = lastSlash;
    if (lastBackslash != NULL && (separator == NULL || lastBackslash > separator)) {
        separator = lastBackslash;
    }

    if (separator != NULL) {
        *separator = '\0';
    } else {
        _directoryPath[0] = '\0';
    }
}

// ================
// AF_File_GetFileNameOnly
// Extracts the file name from a full file path and stores it in the provided buffer.
// This function takes a full file path as input and extracts the file name portion of the path.
// The extracted file name is stored in the provided output buffer, which has a specified size to prevent buffer overflows.
// ================
void AF_File_GetFileNameOnly(const char* _path, char* _outFileName, uint32_t _outFileNameSize) {
    if (_outFileName == NULL || _outFileNameSize == 0) {
        return;
    }
    _outFileName[0] = '\0';

    if (_path == NULL || _path[0] == '\0') {
        return;
    }

    const char* lastSlash = strrchr(_path, '/');
    const char* lastBackslash = strrchr(_path, '\\');
    const char* fileName = _path;

    if (lastSlash != NULL && lastBackslash != NULL) {
        fileName = (lastSlash > lastBackslash) ? (lastSlash + 1) : (lastBackslash + 1);
    } else if (lastSlash != NULL) {
        fileName = lastSlash + 1;
    } else if (lastBackslash != NULL) {
        fileName = lastBackslash + 1;
    }

    snprintf(_outFileName, _outFileNameSize, "%s", fileName);
}

// ================
// AF_File_GetFileStemNoExt
// Extracts the file stem (file name without extension) from a full file path and stores
// it in the provided buffer. This function takes a full file path as input and extracts the file name portion of the path, then removes the file extension if present.
// ================
void AF_File_GetFileStemNoExt(const char* _fileName, char* _outStem, uint32_t _outStemSize) {
    if (_outStem == NULL || _outStemSize == 0) {
        return;
    }
    _outStem[0] = '\0';

    if (_fileName == NULL || _fileName[0] == '\0') {
        return;
    }

    snprintf(_outStem, _outStemSize, "%s", _fileName);
    char* dot = strrchr(_outStem, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
}