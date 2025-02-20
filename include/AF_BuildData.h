#ifndef AF_BUILDDATA_H
#define AF_BUILDDATA_H

#define MAX_BUILDDATA_FILE_PATH 128
// Build game data struct
typedef struct AF_BuildGameData{
    char buildPath[MAX_BUILDDATA_FILE_PATH];

}   AF_BuildGameData;


static inline AF_BuildGameData AF_BuildGameData_ZERO(void){
    AF_BuildGameData returnData;
    returnData.buildPath[0] = '\0';
    return returnData;
}
#endif  // AF_BUILDDATA_H
