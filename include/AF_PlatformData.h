/*
===============================================================================
AF_PLATFORMDATA_H defninitions

Implementation of lighting data struct 
===============================================================================
*/

#ifndef AF_PLATFORMDATA_H
#define AF_PLATFORMDATA_H

#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif


// ===============  Platform Data =============== 
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

// Define the platform mappings
extern const AF_Platform_t AF_Platform_Mappings[];

// =============== Platform Data Struct =============== 

typedef struct AF_PlatformData{
	AF_Platform_e platformType;
} AF_PlatformData;


/*
================
AF_PlatformData_ZERO
// Construct and return the platformdata 
================
*/
static inline AF_PlatformData AF_PlatformData_ZERO(void){
	AF_PlatformData returnPlatformData = {
		.platformType = (AF_Platform_e)0
	};

	return returnPlatformData;
}

// =============== Helper Functions =============== 

/*
================
AF_Platforms_ConvertToCharArray
// Construct and return the platforms as an array of chars
================
*/
inline static void AF_Platforms_ConvertToCharArray(const AF_Platform_t* _mappings, const char** _charArray, uint32_t _size) {
    if (!_mappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _mappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}


#ifdef __cplusplus
}
#endif

#endif  // AF_PLATFORMDATA_H
