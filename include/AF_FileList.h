#ifndef AF_FILELIST_H
#define AF_FILELIST_H
#include "AF_Lib_Define.h"
#define MAX_FILELIST_BUFFER_SIZE 2048
typedef struct AF_FileList {
    char stringBuffer[MAX_FILELIST_BUFFER_SIZE];
    uint32_t numberOfFiles;
    BOOL isSorted;
} AF_FileList;

#endif
