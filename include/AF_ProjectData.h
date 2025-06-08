#ifndef AF_PROJECTDATA_H
#define AF_PROJECTDATA_H

#include "AF_BuildData.h"
#include "AF_RenderingData.h"
#include "AF_PlatformData.h"
#define MAX_PROJECTDATA_FILE_PATH 1024

#define DEFAULT_GAME_NAME "template_game"
#define DEFAULT_ASSETS_DIR "assets"
#define DEFAULT_DATA_DIR "data"
#define DEFAULT_LEVELDATA_FILE_NAME "levelData.dat"
#define DEFAULT_APPDATA_FILE_NAME "appData.dat"
#define DEFAULT_BIN_DIR "bin"


// =============== ProjectData Struct =============== 

// Project data struct
typedef struct AF_ProjectData{
    char name[MAX_PROJECTDATA_FILE_PATH];
    char projectRoot[MAX_PROJECTDATA_FILE_PATH];
    char assetsPath[MAX_PROJECTDATA_FILE_PATH];
    char defaultAppDataPath[MAX_PROJECTDATA_FILE_PATH];
    char defaultScenePath[MAX_PROJECTDATA_FILE_PATH];

    AF_PlatformData platformData;
    AF_BuildGameData buildData;
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
    returnData.defaultAppDataPath[0] = '\0';
    returnData.defaultScenePath[0] = '\0';
    returnData.platformData = AF_PlatformData_ZERO();
    returnData.buildData = AF_BuildGameData_ZERO();

    return returnData;
}


#endif
