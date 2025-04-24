#ifndef AF_PROJECTDATA_H
#define AF_PROJECTDATA_H

#include "AF_BuildData.h"
#include "AF_RenderingData.h"
#include "AF_PlatformData.h"
#define MAX_PROJECTDATA_FILE_PATH 1024


// =============== ProjectData Struct =============== 

// Project data struct
typedef struct AF_ProjectData{
    char name[MAX_PROJECTDATA_FILE_PATH];
    char projectRoot[MAX_PROJECTDATA_FILE_PATH];
    char assetsPath[MAX_PROJECTDATA_FILE_PATH];
    char defaultScene[MAX_PROJECTDATA_FILE_PATH];
    char buildPath[MAX_PROJECTDATA_FILE_PATH];
    AF_PlatformData platformData;
    AF_BuildGameData buildData;
    AF_RenderingData renderingData;
}   AF_ProjectData;



/*
================
Editor_Project_Data_ZERO
// Construct and return the project data
================
*/
static inline AF_ProjectData Editor_Project_Data_ZERO(void){
    AF_ProjectData returnData;
    returnData.name[0] = '\0';
    returnData.projectRoot[0] = '\0';
    returnData.assetsPath[0] = '\0';
    returnData.defaultScene[0] = '\0';
    returnData.buildPath[0] = '\0';
    returnData.platformData = AF_PlatformData_ZERO();
    returnData.buildData = AF_BuildGameData_ZERO();
    returnData.renderingData = AF_AF_RenderingData_ZERO();

    return returnData;
}


#endif
