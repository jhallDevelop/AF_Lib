#include "AF_File.h"
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h> // Main Windows header
#else
#include <dirent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AF_Log.h"

/*
================================
AF_File_OpenFile
Take in a path 
Create some memory to read the data
Return the pointer to the char buffer
================================
*/
FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands){
    if(_path == NULL){
        AF_Log_Error("AF_File_OpenFile: FAILED to open file. _path is NULL\n");
        return 0;
    }
    FILE* f = NULL;
    errno_t err = 0;
    #ifdef _WIN32
        err = fopen_s(&f, _path, _writeCommands);
        if (err == 0) {
            // flip the success on windows
            err = 1;
        }
    #else
        f = fopen(_path, _writeCommands);
        if(f != NULL){
            err = 1;    // true
        }
    #endif

    // A return value of 0 means success. f is now valid.
    if (err == 0) {
        AF_Log_Error("AF_File_OpenFile: FAILED to open file: %s (Error code: %d)\n", _path, err);
        return NULL;
    }

    if (f == NULL) {
        AF_Log_Error("AF_File_OpenFile: FAILED: to open file: Null %s\n", _path);
        return 0;
    } 
    if (ferror(f)) {
        AF_Log_Error("AF_File_OpenFile: Error while opening \n");
		return 0;
    }

    return f;
}

/*
================================
AF_File_PrintTextBuffer
Take in a buffer ptr 
Take in a size of the buffer
Print to console the buffer contents
================================
*/
void AF_File_PrintTextBuffer(FILE* _filePtr){
    //printf("==== PrintTextBuffer ====\n");
    if(_filePtr == NULL){
        AF_Log_Error("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
        return;
    }
    char buf[1024];
    size_t nread;
    // Loop to read the file in chunks
    while ((nread = fread(buf, sizeof(buf[0]), sizeof(buf) - 1, _filePtr)) != 0) {
        buf[nread] = '\0'; // Null-terminate the buffer

        char* s = buf;
        // Process the buffer line by line
        while (s != NULL) {
            char* s_next_line = strchr(s, '\n'); // Find the next newline
            if (s_next_line != NULL) {
                // Null-terminate the current line
                *s_next_line = '\0';
                AF_Log_Error("%s\n", s);  // Print the current line
                s = s_next_line + 1; // Move s to the next line
            } else {
                // No more newlines in the buffer, print the rest
                AF_Log_Error("%s", s);
                s = NULL; // Exit the loop
            }
        }
    }
    if (ferror(_filePtr)) {
        AF_Log_Error("AF_File_PrintTextBuffer: Error while reading \n");
    }
    
}



/*
================================
AF_File_Write
Take in a file ptr, data to put into the file, size of the data 
Create some memory to read the data
Return the pointer to the char buffer
================================
*/
void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize){
    //printf("==== Write data to binary ====\n");
    if(_filePtr == NULL){
        printf("AF_File_WriteFile: FAILED to open file. _filePtr is NULL\n");
        return;
    }
    
    // Write the struct data to the file
    size_t num_written = fwrite(_data, dataSize, 1, _filePtr);
    if (num_written != 1) {
        printf("AF_File_WriteFile: Error writing to file");
    }
    if (ferror(_filePtr)) {
        printf("AF_File_WriteFile: Error while writing \n");
		return;
    }
    fclose(_filePtr);
    if (ferror(_filePtr)) {
        printf("AF_File_WriteFile: Error while closing \n");
    }
}


/*
================================
AF_File_CloseFile
Take in a path 
Create some memory to read the data
Return the pointer to the char buffer
================================
*/
void AF_File_CloseFile(FILE* _filePtr){
     if(_filePtr == NULL){
        printf("AF_File_CloseFile: FAILED to close buffer. _charBuffer is NULL\n");
        return;
    }
    
    fclose(_filePtr);
    if (ferror(_filePtr)) {
        printf("AF_File_CloseFile: Error while closing \n");
    }
    _filePtr = NULL;
    
}

/*
================================
AF_File_CompareItemsByValue

================================
*/
int AF_File_CompareItemsByValue(const void* lhs, const void* rhs) {
    const char* a = *(const char**)lhs; // Cast to char**
    const char* b = *(const char**)rhs; // Cast to char**
    return strcmp(a, b);                // Use strcmp for alphabetical comparison
}

/*
================================
AF_File_OrderAlphabetically
Take in list of files, and return the list ordered alphabetically 
================================
*/
void AF_File_OrderAlphabetically(AF_FileList* _fileList) {

    if (_fileList->numberOfFiles < 2) {
        _fileList->isSorted = AF_TRUE;
        return;
    }

    char tempBuffer[MAX_FILELIST_BUFFER_SIZE];
	snprintf(tempBuffer, MAX_FILELIST_BUFFER_SIZE, "%s", _fileList->stringBuffer);
    //strncpy(tempBuffer, _fileList->stringBuffer, MAX_FILELIST_BUFFER_SIZE);
    tempBuffer[MAX_FILELIST_BUFFER_SIZE - 1] = '\0';

    char* filePointers[MAX_FILELIST_BUFFER_SIZE] = {0};
    uint32_t fileCount = 0;

    char* token = strtok(tempBuffer, ",");
    char* context = NULL;
    while (token != NULL && fileCount < MAX_FILELIST_BUFFER_SIZE) {
        filePointers[fileCount++] = token;
        
        // todo, don't if def
        #ifdef _WIN32
            token = strtok_s(NULL, ",", &context);
        #else
            token = strtok(NULL, ",");
        #endif
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




/*
================================
AF_File_ListFiles
Take in a path 
Take in a buffer to store the data
Return a char list of files in the path
================================
*/
void AF_File_ListFiles(const char *path, AF_FileList* _fileList, af_bool_t _isAlphabetical)
{
#ifdef _WIN32
	// Windows implementation using FindFirstFile and FindNextFile
	AF_Log_Error("AF_File_ListFiles: Windows not implemented\n");
#else
    struct dirent *dp;
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir) 
        return; 

    // Initialize buffer and file count

    size_t bufferPosition = 0;
    // reset the number of files
    _fileList->numberOfFiles = 0;
    while ((dp = readdir(dir)) != NULL)
    {
         // Skip . and ..
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        size_t nameLength = strlen(dp->d_name);

        // Ensure there is space for the file name and a separating character
        if (bufferPosition + nameLength + 2 > MAX_FILELIST_BUFFER_SIZE) {
            printf("Buffer overflow. Stopping file accumulation.\n"); 
            break;
        }

        // Append file name to the buffer
        snprintf(_fileList->stringBuffer + bufferPosition, MAX_FILELIST_BUFFER_SIZE - bufferPosition, "%s,", dp->d_name);
        bufferPosition += nameLength + 1; // Update buffer position (+1 for the newline)

        _fileList->numberOfFiles++;
    }

    // Close directory stream
    closedir(dir);
    if(_isAlphabetical == AF_TRUE){
        AF_File_OrderAlphabetically(_fileList);
    }
#endif
}

/*
====================
AF_File_FileExists
Check a file exists
====================
*/
af_bool_t AF_File_FileExists(const char* _filePath) {
    struct stat buffer;
    return (stat(_filePath, &buffer) == 0);
}

/*
====================
AF_File_MakeDirectory
Make a directory, and return if it was successful
====================
*/
af_bool_t AF_File_MakeDirectory(const char* _filePath) {
    if (mkdir(_filePath, 0777) == -1) {
        AF_Log_Error("AF_Util_MakeFolder: FAILED to make directory %s\n", _filePath);
        return AF_FALSE;
    }
    // if we succeed then we return true
    AF_Log("AF_Util_MakeFolder: SUCCESS: Directory created: %s\n", _filePath);
    return AF_TRUE;
}

/*
=====================
AF_File_SizeOfFile
Get the size of a file
=====================
*/
uint32_t AF_File_GetFileSize(const char* _filePath) {
    struct stat st;
    if (stat(_filePath, &st) != 0) {
        AF_Log_Error("AF_Util: Size of file: Failed to get file size for %s\n", _filePath);
        return 0; // Return 0 if the file does not exist or an error occurs
    }
    return (uint32_t)st.st_size; // Return the size of the file in bytes
}

/*
====================
AF_File_ReadFile
Read file from path into a provided buffer
====================
*/
af_bool_t AF_File_ReadFile(char* _buffer, uint32_t _bufferSize, const char* _filePath, const char* _readCommand) {

    FILE* _fileOpen = fopen(_filePath, _readCommand);
    if (_fileOpen == NULL) {
        AF_Log_Error("AF_Util: Read File: Failed to read file \n%s \nCheck file exists\n\n", _filePath);
        return AF_FALSE;
    }

	//char* buffer = (char*)malloc(fileSize + 1); // +1 for null terminator   
    if(!_buffer) {
        AF_Log_Error("AF_Util: Read File: Memory allocation failed\n");
        fclose(_fileOpen);
        return AF_FALSE;
	}

	size_t bytesRead = fread(_buffer, 1, _bufferSize, _fileOpen);
    if (bytesRead < (size_t)_bufferSize && ferror(_fileOpen)) {
        AF_Log_Error("AF_Util: Read File: Error reading file \n%s\n", _filePath);
        free(_buffer);
        fclose(_fileOpen);
        return AF_FALSE;
    }

	_buffer[bytesRead] = '\0'; // Null-terminate the string

	fclose(_fileOpen);
	return AF_TRUE; // Return the buffer containing the file contents
    
}
