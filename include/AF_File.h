#ifndef AF_FILE_H
#define AF_FILE_H
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_FILELIST_BUFFER_SIZE 1024
typedef struct AF_FileList {
    char stringBuffer[MAX_FILELIST_BUFFER_SIZE];
    uint32_t numberOfFiles;
} AF_FileList;
/*
================================
AF_File_OpenFile
Take in a path 
Create some memory to read the data
Return the pointer to the char buffer
================================
*/
inline static FILE* AF_File_OpenFile(const char* _path, const char* _writeCommands){
    if(_path == NULL){
        printf("AF_File_OpenFile: FAILED to open file. _path is NULL\n");
        return 0;
    }
    FILE* f;
    f = fopen(_path, _writeCommands);//"r+");
    if (f == NULL) {
        printf("AF_File_OpenFile: FAILED: to open file %s\n", _path);
        return 0;
    } 

    printf("AF_File_OpenFile: %s \n", _path );
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
inline static void AF_File_PrintTextBuffer(FILE* _filePtr){
    printf("==== PrintTextBuffer ====\n");
    if(_filePtr == NULL){
        printf("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
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
                printf("%s\n", s);  // Print the current line
                s = s_next_line + 1; // Move s to the next line
            } else {
                // No more newlines in the buffer, print the rest
                printf("%s", s);
                s = NULL; // Exit the loop
            }
        }
    }
    if (ferror(_filePtr)) {
        printf("Error while reading \n");
    }
    printf("==== ==== ==== ====\n");
}



/*
================================
AF_File_Write
Take in a file ptr, data to put into the file, size of the data 
Create some memory to read the data
Return the pointer to the char buffer
================================
*/
inline static void AF_File_WriteFile(FILE* _filePtr, void* _data, size_t dataSize){
    printf("==== Write data to binary ====\n");
    if(_filePtr == NULL){
        printf("AF_File_WriteFile: FAILED to open file. _filePtr is NULL\n");
        return;
    }
    
    // Write the struct data to the file
    size_t num_written = fwrite(_data, dataSize, 1, _filePtr);
    if (num_written != 1) {
        printf("AF_File_WriteFile: Error writing to file");
    }
    fclose(_filePtr);
    if (ferror(_filePtr)) {
        printf("Error while closing \n");
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
inline static void AF_File_CloseFile(FILE* _filePtr){
     if(_filePtr == NULL){
        printf("AF_File_CloseFile: FAILED to close buffer. _charBuffer is NULL\n");
        return;
    }
    printf("AF_File_CloseFile: %p \n", _filePtr );
    fclose(_filePtr);
    if (ferror(_filePtr)) {
        printf("Error while closing \n");
    }
    _filePtr = NULL;
    
}

/*
================================
AF_File_ListFiles
Take in a path 
Take in a buffer to store the data
Return a char list of files in the path
================================
*/

inline static void AF_File_ListFiles(const char *path, AF_FileList* _fileList)
{
    struct dirent *dp;
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir) 
        return; 

    // Initialize buffer and file count

    size_t bufferPosition = 0;

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
}


#endif // AF_File_H