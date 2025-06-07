#include "AF_File.h"
#include <stdint.h>
#include <sys/types.h>
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
    errno_t err = fopen_s(&f, _path, _writeCommands);

    // A return value of 0 means success. f is now valid.
    if (err != 0) {
        AF_Log_Error("AF_File_OpenFile: FAILED to open file %s (Error code: %d)\n", _path, err);
        return NULL;
    }

    if (f == NULL) {
        AF_Log_Error("AF_File_OpenFile: FAILED: to open file %s\n", _path);
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
        
        token = strtok_s(NULL, ",", &context);
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
