/*
===============================================================================
AF_CAPPDATA_H definitions

Definition for the app data that will be shared around
and helper functions
===============================================================================
*/
#ifndef AF_APPDATA_H
#define AF_APPDATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_Lib_API.h"
#include "AF_Time.h"
#include "AF_Window.h"
//#include "AF_ViewportData.h"
#include "AF_Input.h"
#include "AF_Assets.h"
#include "AF_ProjectData.h"
#include "AF_LightingData.h"
#include "AF_RenderingData.h"

#pragma pack(push, 8)  // Set 8-byte alignment
typedef struct AF_AppData {
    // Start with Vec4 since it likely contains floating point values
    // that benefit from strict alignment

    // Place larger structs next
    AF_ECS ecs;            // Struct size depends on definition
    AF_Time time;          // Struct size depends on definition
    AF_Window window;      // We know this is 40 bytes
    AF_Input input;        // Struct size depends on definition
    AF_Assets assets;      // Struct size depends on definition
    AF_ProjectData projectData;  // Struct size depends on definition
    AF_LightingData lightingData;   // struct to hold important cached lighting data
    AF_RenderingData rendererData;   // struct to hold important rendering data
    // Group boolean values together at the end to minimize padding
    bool isRunning;        // 1 byte
    bool isFullscreen;     // 1 byte
    // Add padding to ensure total struct size is multiple of 8
} AF_AppData;
#pragma pack(pop)

static inline void AF_AppData_Init(AF_AppData* _appData){
    if (_appData == NULL) {
		AF_Log_Error("AF_AppData_ZERO: _appData is NULL");
        return;
    }
    _appData->time = AF_Time_ZERO(0);
    _appData->window = AF_Window_ZERO("", 0, 0);
    _appData->input = AF_Input_ZERO();
    _appData->assets = AF_Assets_ZERO();
    _appData->projectData = Editor_Project_Data_ZERO();
    _appData->rendererData = AF_RenderingData_ZERO();
    _appData->lightingData = AF_LightingData_ZERO();
    _appData->isRunning = AF_FALSE;
    _appData->isFullscreen = AF_FALSE;
}


#ifdef __cplusplus
}
#endif

#endif //AF_ASSETS_H
