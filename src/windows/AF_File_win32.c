#include "AF_File.h"
#include "AF_Log.h"

#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================
// Private helpers
// ================================================================

static const char* AF_File_GetEnv(const char* key, char* outBuffer, size_t outSize) {
	if (!key || !outBuffer || outSize == 0) return NULL;
	char* value = NULL;
	size_t len = 0;
	if (_dupenv_s(&value, &len, key) == 0 && value && value[0] != '\0') {
		strncpy_s(outBuffer, outSize, value, _TRUNCATE);
		outBuffer[outSize - 1] = '\0';
		free(value);
		return outBuffer;
	}
	if (value) free(value);
	return NULL;
}

static FILE* AF_File_TryOpen(const char* path, const char* mode) {
	FILE* f = NULL;
	if (fopen_s(&f, path, mode) == 0) return f;
	return NULL;
}

static af_bool_t AF_File_TryChangeDirectory(const char* path) {
	if (!path || path[0] == '\0') return AF_FALSE;
	if (_chdir(path) == 0) {
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
then USERPROFILE-based remapping, then cwd-key remapping.
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
	AF_File_NormalizePathSeparators(tryPath, sizeof(tryPath), _path, '/', '\\');
	if (strcmp(tryPath, _path) != 0) {
		f = AF_File_TryOpen(tryPath, _writeCommands);
		if (f != NULL) return f;
	}

	if (AF_File_PathHasPrefix(tryPath, "\\Users\\") || AF_File_PathHasPrefix(tryPath, "\\users\\")) {
		char userProfile[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (AF_File_GetEnv("USERPROFILE", userProfile, sizeof(userProfile))) {
			const char* rest = tryPath + 7; /* skip \Users\ */
			while (*rest == '\\') rest++;
			char mappedPath[AF_MAX_PATH_CHAR_SIZE] = {0};
			snprintf(mappedPath, sizeof(mappedPath), "%s\\%s", userProfile, rest);
			f = AF_File_TryOpen(mappedPath, _writeCommands);
			if (f != NULL) return f;
		}
	}

	{
		const char* keys[] = {"AF_Editor", "game_projects", NULL};
		char currentCwd[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (_getcwd(currentCwd, sizeof(currentCwd)) != NULL && currentCwd[0] != '\0') {
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
							snprintf(mappedPath, sizeof(mappedPath), "%.*s\\%s", (int)baseLen, currentCwd, suffix);
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
Enumerate directory entries with FindFirstFileA/FindNextFileA.
================================
*/
void AF_File_ListFiles(const char* path, AF_FileList* _fileList, af_bool_t _isAlphabetical) {
	if (MAX_FILELIST_BUFFER_SIZE > 0) _fileList->stringBuffer[0] = '\0';
	_fileList->numberOfFiles = 0;

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

	if (_fileList->numberOfFiles > 0) _fileList->stringBuffer[bufferPosition - 1] = '\0';
	if (_isAlphabetical == AF_TRUE) AF_File_OrderAlphabetically(_fileList);
}

/*
================================
AF_File_MakeDirectory
Create a directory with _mkdir.
================================
*/
af_bool_t AF_File_MakeDirectory(const char* _filePath) {
	if (_mkdir(_filePath) == -1) {
		AF_Log_Error("AF_Util_MakeFolder: FAILED to make directory %s\n", _filePath);
		return AF_FALSE;
	}
	AF_Log("AF_Util_MakeFolder: SUCCESS: Directory created: %s\n", _filePath);
	return AF_TRUE;
}

/*
================================
AF_File_SetWorkingDirectory
Try as-given, normalize separators, USERPROFILE remap,
drive-letter prefix, then cwd-key and cwd-relative fallbacks.
================================
*/
void AF_File_SetWorkingDirectory(const char* _projectRoot) {
	if (_projectRoot == NULL || _projectRoot[0] == '\0') {
		AF_Log_Error("AF_File_SetWorkingDirectory: Invalid project root\n");
		return;
	}

	if (AF_File_TryChangeDirectory(_projectRoot)) return;

	char tryPath[AF_MAX_PATH_CHAR_SIZE];
	AF_File_NormalizePathSeparators(tryPath, sizeof(tryPath), _projectRoot, '/', '\\');
	if (AF_File_TryChangeDirectory(tryPath)) return;

	if (AF_File_PathHasPrefix(tryPath, "\\Users\\") || AF_File_PathHasPrefix(tryPath, "\\users\\")) {
		char userProfile[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (AF_File_GetEnv("USERPROFILE", userProfile, sizeof(userProfile))) {
			const char* rest = tryPath + 7;
			if (rest[0] == '\\') rest++;
			char mappedPath[AF_MAX_PATH_CHAR_SIZE];
			snprintf(mappedPath, sizeof(mappedPath), "%s\\%s", userProfile, rest);
			if (AF_File_TryChangeDirectory(mappedPath)) return;
		}
	}

	if (tryPath[0] == '\\') {
		char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (_getcwd(cwd, sizeof(cwd)) != NULL && cwd[0] != '\0') {
			char currentDrive[4] = { cwd[0], ':', '\0' };
			char drivePrefixed[AF_MAX_PATH_CHAR_SIZE];
			snprintf(drivePrefixed, sizeof(drivePrefixed), "%s%s", currentDrive, tryPath);
			if (AF_File_TryChangeDirectory(drivePrefixed)) return;
		}
	}

	{
		const char* keys[] = {"AF_Editor", "game_projects", NULL};
		for (int i = 0; keys[i] != NULL; ++i) {
			const char* keyLoc = strstr(tryPath, keys[i]);
			if (keyLoc) {
				char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
				if (_getcwd(cwd, sizeof(cwd)) != NULL && cwd[0] != '\0') {
					const char* suffix = keyLoc + strlen(keys[i]);
					while (*suffix == '\\' || *suffix == '/') suffix++;

					char* editorKey = strstr(cwd, "AF_Editor");
					if (editorKey) {
						size_t rootLen = (size_t)(editorKey - cwd + strlen("AF_Editor"));
						char basePath[AF_MAX_PATH_CHAR_SIZE];
						if (rootLen >= sizeof(basePath)) break;
						memcpy(basePath, cwd, rootLen);
						basePath[rootLen] = '\0';
						char mapped[AF_MAX_PATH_CHAR_SIZE];
						if (suffix[0] != '\0')
							snprintf(mapped, sizeof(mapped), "%s\\%s", basePath, suffix);
						else
							snprintf(mapped, sizeof(mapped), "%s", basePath);
						if (AF_File_TryChangeDirectory(mapped)) return;
					}

					char mapped2[AF_MAX_PATH_CHAR_SIZE];
					if (suffix[0] != '\0')
						snprintf(mapped2, sizeof(mapped2), "%s\\%s", cwd, suffix);
					else
						snprintf(mapped2, sizeof(mapped2), "%s", cwd);
					if (AF_File_TryChangeDirectory(mapped2)) return;
				}
			}
		}
	}

	{
		char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
		if (_getcwd(cwd, sizeof(cwd)) != NULL && cwd[0] != '\0') {
			char combined[AF_MAX_PATH_CHAR_SIZE];
			snprintf(combined, sizeof(combined), "%s\\%s", cwd, tryPath);
			if (AF_File_TryChangeDirectory(combined)) return;
		}
	}

	AF_Log_Error("AF_File_SetWorkingDirectory: Failed to set working directory %s\n", _projectRoot);
}

// Provide non-inline exports for script binding and external module use.
AF_LIB_API af_bool_t AF_File_FileExists(const char* _filePath) {
	struct stat buffer;
	return (stat(_filePath, &buffer) == 0) ? AF_TRUE : AF_FALSE;
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

AF_LIB_API uint32_t AF_File_GetFileSize(const char* _filePath) {
	struct stat st;
	if (stat(_filePath, &st) != 0) {
		AF_Log_Error("AF_Util: Size of file: Failed to get file size for %s\n", _filePath);
		return 0;
	}
	return (uint32_t)st.st_size;
}
