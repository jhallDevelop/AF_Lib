#ifndef AF_PROJECTDATA_H
#define AF_PROJECTDATA_H

#include "AF_BuildData.h"
#include "AF_RenderingData.h"
#include "AF_PlatformData.h"

#define DEFAULT_GAME_NAME "template_game"
#define DEFAULT_ASSETS_DIR "assets"
#define DEFAULT_DATA_DIR "data"
#define DEFAULT_LEVELDATA_FILE_NAME "levelData.dat"
#define DEFAULT_APPDATA_FILE_NAME "appData.json"
#define DEFAULT_BIN_DIR "bin"


// =============== ProjectData Struct =============== 

// Project data struct
typedef struct AF_ProjectData{
    char name[AF_MAX_PATH_CHAR_SIZE];
    char projectRoot[AF_MAX_PATH_CHAR_SIZE];
    char assetsPath[AF_MAX_PATH_CHAR_SIZE];
    char defaultAppDataPath[AF_MAX_PATH_CHAR_SIZE];
    char defaultScenePath[AF_MAX_PATH_CHAR_SIZE];
    // Deferred scene change: set by scripts, processed by the host game loop AFTER all scripts return
    char pendingScenePath[AF_MAX_PATH_CHAR_SIZE];
    af_bool_t hasPendingSceneChange;

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
    returnData.pendingScenePath[0] = '\0';
    returnData.hasPendingSceneChange = AF_FALSE;
    returnData.platformData = AF_PlatformData_ZERO();
    returnData.buildData = AF_BuildGameData_ZERO();

    return returnData;
}




#endif
