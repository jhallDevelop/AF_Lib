#ifndef AF_PROJECTDATA_H
#define AF_PROJECTDATA_H

#include "AF_BuildData.h"
#define MAX_PROJECTDATA_FILE_PATH 1024

typedef enum AF_Platform_e {
    AF_PLATFORM_LINUX = 0,
    AF_BUILD_TYPE_OSX = 1,
    AF_BUILD_TYPE_Win64 = 2,
    AF_BUILD_TYPE_N64  = 3 
} AF_Platform_e;

typedef struct AF_Platform_t{ 
    AF_Platform_e key;
    const char* name;
} AF_Platform_t;

#define AF_PLATFORM_COUNT 4
static const AF_Platform_t AF_Platform_Mappings[AF_PLATFORM_COUNT] = {
    {AF_PLATFORM_LINUX, "LINUX"},
    {AF_BUILD_TYPE_OSX, "OSX"},
    {AF_BUILD_TYPE_Win64, "Win64"},
    {AF_BUILD_TYPE_N64, "N64"}
};

// Define the key mappings
extern const AF_Platform_t AF_Platform_Mappings[];

// Project data struct
typedef struct AF_ProjectData{
    char name[MAX_PROJECTDATA_FILE_PATH];
    char projectRoot[MAX_PROJECTDATA_FILE_PATH];
    char assetsPath[MAX_PROJECTDATA_FILE_PATH];
    char defaultScene[MAX_PROJECTDATA_FILE_PATH];
    char buildPath[MAX_PROJECTDATA_FILE_PATH];
    AF_Platform_e buildPlatformType;
    AF_BuildGameData buildData;
}   AF_ProjectData;

/*
================
AF_Platforms_ConvertToCharArray
// Construct and return the platforms as an array of chars
================
*/
inline static void AF_Platforms_ConvertToCharArray(const AF_Platform_t* _platformMappings, const char** _charArray, uint32_t _size) {
    if (!_platformMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _platformMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}


static inline AF_ProjectData Editor_Project_Data_ZERO(void){
    AF_ProjectData returnData;
    returnData.name[0] = '\0';
    returnData.projectRoot[0] = '\0';
    returnData.assetsPath[0] = '\0';
    returnData.defaultScene[0] = '\0';
    returnData.buildPath[0] = '\0';
    returnData.buildPlatformType = (AF_Platform_e)0;
    returnData.buildData = AF_BuildGameData_ZERO();
    return returnData;
}


#endif
