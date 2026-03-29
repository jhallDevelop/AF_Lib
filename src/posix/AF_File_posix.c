#include "AF_File.h"
#include "AF_Log.h"

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================
// Private helpers
// ================================================================

static const char* AF_File_GetEnv(const char* key, char* outBuffer, size_t outSize) {
	if (!key || !outBuffer || outSize == 0) return NULL;
	const char* value = getenv(key);
	if (value && value[0] != '\0') {
		strncpy(outBuffer, value, outSize);
		outBuffer[outSize - 1] = '\0';
		return outBuffer;
	}
	return NULL;
}

static FILE* AF_File_TryOpen(const char* path, const char* mode) {
	return fopen(path, mode);
}

static af_bool_t AF_File_TryChangeDirectory(const char* path) {
	if (!path || path[0] == '\0') return AF_FALSE;
	if (chdir(path) == 0) {
		AF_Log("AF_File_SetWorkingDirectory: SUCCESS: %s\n", path);
		return AF_TRUE;
	}
	return AF_FALSE;
}

static void AF_File_NormalizePathSeparators(char* outPath, size_t outSize, const char* inPath, char from, char to) {
	if (!outPath || !inPath || outSize == 0) return;
	size_t i = 0;
	for (; i + 1 < outSize && inPath[i] != '\0'; ++i) {
		char c = inPath[i];
		outPath[i] = (c == from ? to : c);
	}
	outPath[i] = '\0';
}

static af_bool_t AF_File_PathHasPrefix(const char* path, const char* prefix) {
	if (!path || !prefix) return AF_FALSE;
	size_t prefixLen = strlen(prefix);
	return (strlen(path) >= prefixLen && strncmp(path, prefix, prefixLen) == 0) ? AF_TRUE : AF_FALSE;
}

// ================================================================
// Public platform implementations
// ================================================================

/*
================================
AF_File_OpenFile
Try original path, then normalize separators,
then HOME-based remapping, then cwd-key remapping.
================================
*/
FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands) {
	if (_path == NULL) {
		AF_Log_Error("AF_File_OpenFile: FAILED to open file. _path is NULL\n");
		return NULL;
	}

	FILE* f = AF_File_TryOpen(_path, _writeCommands);
	if (f != NULL) return f;

	char tryPath[AF_MAX_PATH_CHAR_SIZE] = {0};
	AF_File_NormalizePathSeparators(tryPath, sizeof(tryPath), _path, '\\', '/');
	if (strcmp(tryPath, _path) != 0) {
		f = AF_File_TryOpen(tryPath, _writeCommands);
		if (f != NULL) return f;
	}

	if (AF_File_PathHasPrefix(tryPath, "/Users/") || AF_File_PathHasPrefix(tryPath, "/users/") ||
		AF_File_PathHasPrefix(tryPath, "\\Users\\") || AF_File_PathHasPrefix(tryPath, "\\users\\")) {
		char userHome[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (AF_File_GetEnv("HOME", userHome, sizeof(userHome))) {
			const char* rest = tryPath + 7; // skip /Users/ (7 chars)
			while (*rest == '\\' || *rest == '/') rest++;
			char mappedPath[AF_MAX_PATH_CHAR_SIZE] = {0};
			snprintf(mappedPath, sizeof(mappedPath), "%s/%s", userHome, rest);
			f = AF_File_TryOpen(mappedPath, _writeCommands);
			if (f != NULL) return f;
		}
	}

	{
		const char* keys[] = {"AF_Editor", "game_projects", NULL};
		char currentCwd[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (getcwd(currentCwd, sizeof(currentCwd)) != NULL && currentCwd[0] != '\0') {
			for (int i = 0; keys[i] != NULL; ++i) {
				const char* keyLoc = strstr(tryPath, keys[i]);
				if (!keyLoc) keyLoc = strstr(_path, keys[i]);
				if (keyLoc) {
					const char* suffix = keyLoc + strlen(keys[i]);
					while (*suffix == '\\' || *suffix == '/') suffix++;
					const char* localKey = strstr(currentCwd, keys[i]);
					if (localKey) {
						size_t baseLen = (size_t)(localKey - currentCwd + strlen(keys[i]));
						char mappedPath[AF_MAX_PATH_CHAR_SIZE] = {0};
						if (suffix[0] != '\0')
							snprintf(mappedPath, sizeof(mappedPath), "%.*s/%s", (int)baseLen, currentCwd, suffix);
						else
							snprintf(mappedPath, sizeof(mappedPath), "%.*s", (int)baseLen, currentCwd);
						f = AF_File_TryOpen(mappedPath, _writeCommands);
						if (f != NULL) return f;
					}
				}
			}
		}
	}

	AF_Log_Error("AF_File_OpenFile: FAILED to open file: %s\n", _path);
	return NULL;
}

/*
================================
AF_File_ListFiles
Enumerate directory entries with opendir/readdir.
================================
*/
void AF_File_ListFiles(const char* path, AF_FileList* _fileList, af_bool_t _isAlphabetical) {
	if (MAX_FILELIST_BUFFER_SIZE > 0) _fileList->stringBuffer[0] = '\0';
	_fileList->numberOfFiles = 0;

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

	if (_fileList->numberOfFiles > 0) _fileList->stringBuffer[bufferPosition - 1] = '\0';
	if (_isAlphabetical == AF_TRUE) AF_File_OrderAlphabetically(_fileList);
}

/*
================================
AF_File_MakeDirectory
Create a directory with full permissions.
================================
*/
af_bool_t AF_File_MakeDirectory(const char* _filePath) {
	if (mkdir(_filePath, 0777) == -1) {
		AF_Log_Error("AF_Util_MakeFolder: FAILED to make directory %s\n", _filePath);
		return AF_FALSE;
	}
	AF_Log("AF_Util_MakeFolder: SUCCESS: Directory created: %s\n", _filePath);
	return AF_TRUE;
}

/*
================================
AF_File_SetWorkingDirectory
Try as-given, then normalize separators, then cwd-relative.
================================
*/
void AF_File_SetWorkingDirectory(const char* _projectRoot) {
	if (_projectRoot == NULL || _projectRoot[0] == '\0') {
		AF_Log_Error("AF_File_SetWorkingDirectory: Invalid project root\n");
		return;
	}

	if (AF_File_TryChangeDirectory(_projectRoot)) return;

	char tryPath[AF_MAX_PATH_CHAR_SIZE];
	AF_File_NormalizePathSeparators(tryPath, sizeof(tryPath), _projectRoot, '\\', '/');
	if (AF_File_TryChangeDirectory(tryPath)) return;

	char cwd[AF_MAX_PATH_CHAR_SIZE];
	if (getcwd(cwd, sizeof(cwd)) != NULL && cwd[0] != '\0') {
		char combined[AF_MAX_PATH_CHAR_SIZE];
		snprintf(combined, sizeof(combined), "%s/%s", cwd, tryPath);
		if (AF_File_TryChangeDirectory(combined)) return;
	}

	AF_Log_Error("AF_File_SetWorkingDirectory: Failed to set working directory %s\n", _projectRoot);
}

AF_LIB_API void AF_File_CloseFile(FILE* _filePtr) {
	if (_filePtr == NULL) {
		AF_Log_Error("AF_File_CloseFile: FAILED to close buffer. _filePtr is NULL\n");
		return;
	}

	if (ferror(_filePtr)) {
		AF_Log_Warning("AF_File_CloseFile: File had errors before closing\n");
	}

	if (fclose(_filePtr) != 0) {
		AF_Log_Error("AF_File_CloseFile: Error while closing file\n");
	}
}

AF_LIB_API af_bool_t AF_File_FileExists(const char* _filePath) {
	if (_filePath == NULL || _filePath[0] == '\0') {
		return AF_FALSE;
	}

	struct stat fileStat;
	if (stat(_filePath, &fileStat) == 0) {
		return AF_TRUE;
	}
	return AF_FALSE;
}

AF_LIB_API uint32_t AF_File_GetFileSize(const char* _filePath) {
	if (_filePath == NULL || _filePath[0] == '\0') {
		return 0;
	}

	struct stat fileStat;
	if (stat(_filePath, &fileStat) != 0) {
		return 0;
	}

	if (fileStat.st_size < 0) {
		return 0;
	}

	return (uint32_t)fileStat.st_size;
}
