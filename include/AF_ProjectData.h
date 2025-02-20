#ifndef AF_PROJECTDATA_H
#define AF_PROJECTDATA_H

#include "AF_BuildData.h"
#define MAX_PROJECTDATA_FILE_PATH 128

// Project data struct
typedef struct AF_ProjectData{
    char name[MAX_PROJECTDATA_FILE_PATH];
    char projectRoot[MAX_PROJECTDATA_FILE_PATH];
    char assetsPath[MAX_PROJECTDATA_FILE_PATH];
    AF_BuildGameData buildData;
}   AF_ProjectData;


static inline AF_ProjectData Editor_Project_Data_ZERO(void){
    AF_ProjectData returnData;
    returnData.name[0] = '\0';
    returnData.projectRoot[0] = '\0';
    returnData.assetsPath[0] = '\0';
    returnData.buildData = AF_BuildGameData_ZERO();
    return returnData;
}
#endif
